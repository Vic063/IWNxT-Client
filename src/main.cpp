//========================================================
// Project: IWNxT
// Component: IWNxT Client 
//
// Initial Author: Vico
// Date: 30 Dec 2013
// File: main.cpp
//
// Purpose: Simple client to demonstrate the usage of
// the IWNxT library
//========================================================

#include <iostream>
#include "IWNxT_API.h"

using namespace std;

struct friend_t {
	SFriend s;
	EPersonaState state;
};

/* external variables that need to be be declared */
HMODULE iwnxtMod;
initAPI InitializeAPI;
userID getUserID;
userName getUsername;
friendCount getFriendCount;
friendByIndex getFriendByIndex;
inviteFriend InviteFriend;
inviteRequest InviteCallback;
friendPersonaState getFriendPersonaState;
shutdownAPI ShutdownAPI;

/* global variables, yeah I know, it's ugly */
friend_t *friendsTab;
unsigned int count;

const char* ePersonaStateToStr( EPersonaState state )
{
	switch (state)
	{
	case k_EPersonaStateOffline:
		return "Offline";

	case k_EPersonaStateOnline:
		return "Online";

	case k_EPersonaStateBusy:
		return "Busy";

	case k_EPersonaStateAway:
		return "Away";

	case k_EPersonaStateSnooze:
		return "Away (for a long time)";

	case k_EPersonaStateLookingToTrade:
		return "Online";

	case k_EPersonaStateLookingToPlay:
		return "Online";

	case k_EPersonaStateMax:
		return "Online";

	default:
		return "Unknown"; /* Should never happen */
	}
}

DWORD WINAPI updateThread( LPVOID lParams )
{
	EPersonaState state;

	while (true)
	{
		/* Sleep during 10 seconds */
		Sleep(10000);

		for (unsigned int i = 0; i < count; ++i)
		{
			state = getFriendPersonaState(friendsTab[i].s.userID);
			if (state != friendsTab[i].state)
			{
				friendsTab[i].state = state;
				cout << friendsTab[i].s.name << " is now " << ePersonaStateToStr(state) << "." << endl;
			}
		}
	}

	ExitThread(0);
}

void freeLibrary()
{
	if (ShutdownAPI != NULL)
		ShutdownAPI();

	cout << "Freeing IWNxT.dll." << endl;
	FreeLibrary(iwnxtMod);
}

/* Catch Ctrl+ actions */
BOOL CtrlHandler( DWORD fdwCtrlType ) 
{
	switch (fdwCtrlType)
	{
		case CTRL_C_EVENT:
			freeLibrary();
			exit(0);
	}

	return FALSE;
}

int main( void )
{
	uint64_t UID;
	EPersonaState state;
	SFriend s;
	HANDLE hThread;

	/* First, load the IWNxT library */
	iwnxtMod = LoadLibrary(L"IWNxT.dll");
	if (iwnxtMod == NULL)
	{
		DWORD dwRet = GetLastError();
		cerr << "Cannot open IWNxT.dll (" << dwRet << ")." << endl;
		return 1;
	}

	/* If it started correctly, then the dll entry point is loaded and we have nothing to do ;) */
	InitializeAPI = (initAPI)GetProcAddress(iwnxtMod, "InitializeAPI");
	ShutdownAPI = (shutdownAPI)GetProcAddress(iwnxtMod, "ShutdownAPI");
	if (InitializeAPI == NULL || ShutdownAPI == NULL)
	{
		cerr << "Cannot load API core." << endl;		
		freeLibrary();
		return 1;
	}

	/* Initialize the API, it's mandatory to use others methods */
	InitializeAPI();

	/* Add a handler which will check if the user typed Ctrl+C combination */
	if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE))
	{
		cerr << "Cannot install handler." << endl;
		freeLibrary();
		return 1;
	}

	getUserID = (userID)GetProcAddress(iwnxtMod, "GetUserID");
	if (getUserID == NULL)
	{
		cerr << "Cannot get User ID." << endl;
		freeLibrary();
		return 1;
	}

	UID = getUserID();
	cout << "User ID is " << UID << endl;

	getFriendCount = (friendCount)GetProcAddress(iwnxtMod, "GetFriendCount");
	getFriendByIndex = (friendByIndex)GetProcAddress(iwnxtMod, "GetFriendByIndex");
	getFriendPersonaState = (friendPersonaState)GetProcAddress(iwnxtMod, "GetFriendPersonaState");

	if (getFriendCount == NULL || getFriendByIndex == NULL || getFriendPersonaState == NULL)
	{
		cerr << "Cannot get Friend count or get Friend by index or get Friend state." << endl;
		freeLibrary();
		return 1;
	}

	count = getFriendCount();
	cout << "You have " << count << " friend(s)!" << endl;
	/* Initialize our friendsTab variable and store all friends */
	friendsTab = new friend_t[count];

	for (unsigned int i = 0; i < count; ++i)
	{
		s = getFriendByIndex(i);
		state = getFriendPersonaState(s.userID);
		friendsTab[i].s = s;
		friendsTab[i].state = state;

		if (s.name == NULL)
			cerr << "Unable to retreive friend's information at index " << i + 1 << " :(" << endl;
		else
			cout << "Friend " << i + 1 << ":" << endl << "Name: " << s.name << endl << "IWNxTID: " << s.userID << ". User is currently " << ePersonaStateToStr(state) << "." << endl;
	}

	/* Start a thread which will update our friend's status */
	hThread = CreateThread(NULL, 0, updateThread, NULL, 0, NULL);
	if (hThread == NULL)
	{
		DWORD dwRet = GetLastError();
		cerr << "Cannot create update thread (" << dwRet << "). You won't be advertised when one of your friends changes his state." << endl;
	}

	/* Sleep during 2 minutes */
	Sleep(12000);
	TerminateThread(hThread, 0);

	/* Don't forget to free the memory took by the library! */
	freeLibrary();

	return 0;
}