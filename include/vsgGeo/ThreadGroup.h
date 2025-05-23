#pragma once

/* vsgGeo - A collection of geoscientific extensions to VulkanSceneGraph.
Copyright 2025 dGB Beheer B.V.

vsgGeo is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#include <vsgGeo/Common.h>


namespace vsgGeo
{

/* Holds a pool of threads of type T. */

template<class T> class ThreadGroup : public osg::Referenced
{
public:

    osg::ref_ptr<T>				getThread();
    /* Provided thread should normally be returned to the pool. */
    void					returnThread(T *thread);
    /* Return thread to pool. */

    static osg::ref_ptr<ThreadGroup<T> >	getInst();
    /*Static instance of the group. This is normally alive as long as someone
      wants it, and recreated if there is none. */

private:
    std::vector<osg::ref_ptr<T> >		_tasks;
    OpenThreads::Mutex				_taskLock;

    static osg::observer_ptr<ThreadGroup<T> >	_inst;
};


template<class T> osg::observer_ptr<ThreadGroup<T> > ThreadGroup<T>::_inst = 0;

template<class T> osg::ref_ptr<ThreadGroup<T> > ThreadGroup<T>::getInst()
{
    osg::ref_ptr<ThreadGroup<T> > res = 0;
    _inst.lock(res);
    if ( res.valid() )
	return res;

    res = new ThreadGroup<T>;

    /*This is not 100% ideal, but it is safe. There may be a data race. In such case
    There may be two groups created, but who cares, as the data and threads will be
    fully managed. */

    _inst = res;
    return res;
}


template<class T> osg::ref_ptr<T> ThreadGroup<T>::getThread()
{
    osg::ref_ptr<T> res = 0;
    _taskLock.lock();
    if ( _tasks.size() )
    {
        res = _tasks.back();
        _tasks.pop_back();
    }
    _taskLock.unlock();
    if ( !res )
    {
	res = new T(*this);
        res->start();
    }

    return res;
}


template<class T> void ThreadGroup<T>::returnThread(T *task)
{
    _taskLock.lock();
    _tasks.push_back( task );
    _taskLock.unlock();
}


//=============================================================================


/* Practical base class for pool threads of type T:
   (1) Implement any kind of "set(.)" function to pass variables. You have to
       call beginSetFunction(.) at its begin and endSetFunction() at its end.
   (2) Reimplement the doWork() function to have your parallel code executed.
*/

template<class T> class GroupThread : public osg::Referenced, public OpenThreads::Thread
{
public:

			GroupThread(ThreadGroup<T>& tg)
				: _stopFlag(false)
				, _isSet(false)
				, _group(tg)
			{}

    void		run() override
			{
			    _lock.lock();
			    while ( !_stopFlag )
			    {
				while ( !_stopFlag && !_isSet )
				    _cond.wait( &_lock );
				if ( _stopFlag || !_isSet )
				    continue;

				doWork();

				// Order is important here, as we must return
				// the task before we update ready-count,
				// as we may be deleted otherwise
				_isSet = false;
				_group.returnThread( dynamic_cast<T*>(this) );
				if ( _readyCount )
				    _readyCount->completed();
			    }
			    _lock.unlock();
			}
protected:

    virtual		~GroupThread()
			{
			    _lock.lock();
			    _stopFlag = true;
			    _cond.signal();
			    _lock.unlock();

			    while( isRunning() )
				OpenThreads::Thread::YieldCurrentThread();
			}

    void		beginSetFunction(OpenThreads::BlockCount* ready=0)
			{
			    _lock.lock();
			    _readyCount = ready;
			}

    void		endSetFunction()
			{
			    _isSet = true;
			    _cond.signal();
			    _lock.unlock();
			}

    virtual void	doWork()				 {};

    /* This is a reference as it cannot (i.e. should) not be during the
       lifecycle of the object. If it is in the group's pool, it will be
       deleted by the pool. As the layered texture has reffed the pool
       during it's life, the pool will stay alive. */
    ThreadGroup<T>&		_group;

    OpenThreads::BlockCount*	_readyCount;
    OpenThreads::Condition	_cond;
    OpenThreads::Mutex		_lock;
    bool			_stopFlag;
    bool			_isSet;
};


} // namespace vsgGeo
