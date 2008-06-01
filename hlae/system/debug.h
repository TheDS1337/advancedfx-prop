#pragma once
#if 0
#define HLAE_DEBUG_READY 1

//  debug.h - Debug system
//  Copyright (c) Half-Life Advanced Effects project

//  Last changes:
//	2008-05-30 by dominik.matrixstorm.com

//  First changes:
//	2008-05-28 by dominik.matrixstorm.com

//
//  A note to programers about threading safety:
//
//    If you do any changes to the DebugMaster or other sensitive classes,
//    do them carefully to ensure that functions that are guaranteed to be
//    threadsafe remain threadsafe.
//
//    Functions not explicetly marked as threadsafe must not be asumed to be
//    threadsafe.
//
//  Some of the classes are extensively documented, make sure to update
//  the documentation in case your changes require that, also be aware that
//  users of the clase might already make asumptions from the previous
//  documenation.
//

using namespace System;
using namespace System::Windows::Forms;

namespace hlae {
namespace debug {

enum class DebugMessageType
{
	none,
	DMT_ERROR,
	DMT_WARNING,
	DMT_INFO,
	DMT_VERBOSE,
	DMT_DEBUG
};

enum class DebugMessageState
{
	none,
	DMS_POSTED,
	DMS_FAILED,
	DMS_IGNORED
};

enum class DebugFilterSetting
{
	DFS_DEFAULT=0,
	DFS_IGNORE,
	DFS_NODROP
};

enum class LogFileState
{
	none,
	LFS_NOTSET,
	LFS_OK,
	LFS_ERROR,
};

enum class DebugQueueState
{
	none,
	DQS_OK,
	DQS_WASFULL,
	DQS_WASCONGESTED
};

enum class DebugAttachState
{
	none,
	DAS_ATTACHED,
	DAS_DEATTACHED,
	DAS_ATTACHFAILED
};

ref class DebugMaster; // forward decleration

delegate void MasterDeattachDelegate( DebugMaster ^debugMaster );
delegate DebugMessageState MasterMessageDelegate( DebugMaster ^debugMaster, System::String ^debugMessage, DebugMessageType debugMessageType ); 

ref class DebugListenerBridge
{
public:
	void MasterDeattach( DebugMaster ^debugMaster );
	DebugMessageState MasterMessage( DebugMaster ^debugMaster, System::String ^debugMessage, DebugMessageType debugMessageType ); 
public:
	DebugListenerBridge( MasterDeattachDelegate ^masterDeattachDelegate, MasterMessageDelegate ^masterMessageDelegate );
private:
	MasterDeattachDelegate ^masterDeattachDelegate;
	MasterMessageDelegate ^masterMessageDelegate;
};

//  DebugListener:
//
//  Abstract base class for attaching to one or several debugMasters and recieving.
//  Deriving classes should only override OnSpewMessage 
ref class DebugListener abstract
{
public:
	//
	// Threadsafe functions:
	//

	DebugAttachState Attach( DebugMaster ^debugMaster );
	void Deattach( DebugMaster ^debugMaster );
	void Deattach();

	DebugAttachState GetAttachState( DebugMaster ^debugMaster );
	DebugAttachState GetAttachState();

	//
	// non threadsafe functions:
	//

	DebugListener( bool bInterLockOnSpewMessage );
	DebugListener( bool bInterLockOnSpewMessage, DebugMaster ^debugMaster );
	~DebugListener();

	//  Override this to process a incoming message
	//  IF bInterLockOnSpewMessage was set true on class creation:
	//  The call is already interlocked, thus you can asume the function is not
	//  called again before you finihed processing.
	virtual DebugMessageState OnSpewMessage(
		DebugMaster ^debugMaster,
		System::String ^debugMessage,
		DebugMessageType debugMessageType
	);

private:
	bool bInterLockOnSpewMessage;
	System::Object ^spewMessageSyncer; // object pointer is used for locking
	DebugListenerBridge ^debugListenerBridge;
	System::Collections::Generic::LinkedList<DebugMaster ^> ^debugMasters; // object pointer is used for locking

	void InitDebugListener( bool bInterLockOnSpewMessage );

	void MasterDeattach( DebugMaster ^debugMaster ); // threadsafe
	DebugMessageState MasterMessage( DebugMaster ^debugMaster, System::String ^debugMessage, DebugMessageType debugMessageType ); // threadsafe
};

(*ref class DebugScheduler
{
	// DebugMasters use these to register and unregister autonomically
	MasterAdd( );
	MasterRemove( );
};*)


//  DebugMaster:
//
//  The debug system handles debug messages and how to save them to a file and
//  deliver them to a UI display (DebugControl).
//
//  Messages that have been successfuly enqued won't be dropped drom the que.
//  This is intended by design, since earlier messages are consindered more
//  important than later ones.
//
//  By default only DMT_ERROR messages may not be dropped, since not dropping
//  a message can have significant performance hits (see "Incoming messages").
//
//  Incoming messages (PostMessages()) are handled in the following order:
//    In case the filter is set to ignore the message, it is ignored.
//	  If the que is full and the filter allows dropping the message, the
//      message is dropped.
//    If the que is full and the filter is set to not allow dropping the
//       message, the que is delivered and if message still does not fit into
//       the now empty que it is delivered directly, otherwise it is enqued.
//       Please be aware that this situation means a significant performance
//       hit:
//       Incoming messages are either dropped or the PostMessage call is
//       blocked until the delivery of the current message has been
//       carried out.
//    Otherwise the message is enqued.
//
//  The message que is "full" when:
//    - Adding the message would void maxMessages
//    - Adding the message would void maxSumLengths
//
//  The message que is "congested" when:
//    - The que is full and a incoming message may not be dropped
//
//  Delivery of the que is trigered by the following events:
//    - The que is congested
//    - thresholdMaxMessages has been hit
//    - thresholdMaxSumLengths has been hit
//
//  Delivery works as follows:
//  A worker thread is triggered, pops out a message of the que and delivers it.
//  The work thread repeats this operation until one of the following conditions are met:
//    - number of qued messages <= thresholdMinMessages
//    - sum of the qued messages' lengths <= thresholdMinSumLengths
//    - in case the delivery was requested due to 
ref class DebugMaster
{
public:
	//
	// Threadsafe functions:
	//

	DebugMessageState PostMessage( System::String ^debugMessage, DebugMessageType debugMessageType );

	//  Result:
	//    the last tracked DebugQueueState since the last time this function was called.
	DebugQueueState GetLastQueueState();

	DebugFilterSetting GetFilter( DebugMessageType debugMessageType );
	void SetFilter( DebugMessageType debugMessageType, DebugFilterSetting debugFilterSetting );

	DebugAttachState RegisterListener( DebugListenerBridge ^debugListenerBridge );
	void UnregisterListener( DebugListenerBridge ^debugListenerBridge );

	//
	//  non threadsafe functions:
	//

	DebugMaster();

	~DebugMaster();

private:
	//
	// Threadsafe functions:
	//


	//
	//  non threadsafe functions:
	//

	void InitDebugMaster(
		unsigned int maxMessages,
		unsigned int thresholdMinMessages,
		unsigned int thresholdMaxMessages,
		unsigned int maxSumLengths,
		unsigned int thresholdMinSumLengths,
		unsigned int thresholdMaxSumLengths,
		unsigned int thresholdMinIdleMilliSeconds
	); 

private:
	// access to those is interlocked using messageQue:
	unsigned int maxMessages;
	unsigned int thresholdMinMessages;
	unsigned int thresholdMaxMessages;
	unsigned int maxSumLengths;
	unsigned int thresholdMinSumLengths;
	unsigned int thresholdMaxSumLengths;
	unsigned int thresholdMinIdleMilliSeconds;
	unsigned int curSumLenghts;

	DebugQueueState debugQueueState;
	System::Object ^debugQueueStateSyncer; // object pointer is used for locking read-set operations to debugQueueState

	DebugFilterSetting filterError;
	DebugFilterSetting filterWarning;
	DebugFilterSetting filterInfo;
	DebugFilterSetting filterVerbose;
	DebugFilterSetting filterDebug;

	System::Collections::Generic::Queue<System::String ^> ^messageQue; // object pointer is used for locking

	System::Collections::Generic::LinkedList<DebugListenerBridge ^> ^listenerBridges; // object pointer is used for locking, also specific list members are used for interlocking (so they won't get removed while a message is being posted)
};

}	// namespace debug
}	// namespace hlae
#endif