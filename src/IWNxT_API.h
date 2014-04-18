//========================================================
// Project: IWNxT
// Component: IWNxT base code 
//
// Initial Author: Vico
// Date: 28 Jan 2014
// File: IWNxT_API.h
//
// Purpose: Header file used by all games which have the
// IWNxT integration
//========================================================

#ifndef _IWNXT_API_H
#define _IWNXT_API_H

#include <Windows.h>
#include <cstdint>

/* Define all custom structures used in the IWNxT API */
typedef struct {
	uint64_t userID;
	char *name;
}SFriend;

enum EPersonaState
{
	k_EPersonaStateOffline = 0,			// friend is not currently logged on
	k_EPersonaStateOnline = 1,			// friend is logged on
	k_EPersonaStateBusy = 2,			// user is on, but busy
	k_EPersonaStateAway = 3,			// auto-away feature
	k_EPersonaStateSnooze = 4,			// auto-away for a long time
	k_EPersonaStateLookingToTrade = 5,	// Online, trading
	k_EPersonaStateLookingToPlay = 6,	// Online, wanting to play
	k_EPersonaStateMax,
};

/* Define all exported functions from the IWNxT API */
typedef uint64_t (*userID)( void );
typedef const std::string (*userName)( void );
typedef void* (*encryptedAppTicket)( void*, int, int, uint32_t* );
typedef unsigned int (*friendCount)( void );
typedef SFriend (*friendByIndex)( unsigned int iFriend );
typedef void (*initAPI)( void );
typedef bool (*inviteFriend)( uint64_t friendID );
typedef bool (*inviteRequest)( SFriend *s );
typedef EPersonaState (*friendPersonaState)( uint64_t friendID );
typedef void (*shutdownAPI)( void );

/* Define all global variables to be used everywhere */
extern HMODULE iwnxtMod;
extern userID getUserID;
extern userName getUsername;
extern friendCount getFriendCount;
extern friendByIndex getFriendByIndex;
extern initAPI InitializeAPI;
extern inviteFriend InviteFriend;
extern inviteRequest InviteCallback;
extern friendPersonaState getFriendPersonaState;
extern shutdownAPI ShutdownAPI;

/*
Here is a quick reminder of all exported functions from the IWNxT API
- GetUserID: retrieve the current user identifier
- GetUsername: retrieve the current user name
- GetFriendCount: retrieve the number of friends you have (only the number of friends, nothing more)
- GetFriendByIndex: retrieve a SFriend struct of the friend defined at index 'iFriend'. The struct contains the name of the user and the ID of the user
- GetEncryptedAppTicket: create a custom encrypted application ticket containing the buffer passed as argument 1
- InitializeAPI: initialize the IWNxT API
- InviteFriend: send an invite request to the specified friend. Returns 1 if succeed, 0 otherwise
- InviteCallback: check if we have received an invitation from a friend. The given SFriend struct is populated if the function returns true
- GetFriendPersonaState: retrieve the state of the specified friend. Check the EPersonaState struct to know each differents values
- ShutdownAPI: shut down the IWNxT API
*/

#endif /* _IWNXT_API_H */