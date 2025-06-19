/* IBM_PROLOG_BEGIN_TAG                                                      */
/* This is an automatically generated prolog.                                */
/*                                                                           */
/* Licensed Materials -Property of IBM                                       */
/* 5737-F34                                                                  */
/*                                                                           */
/* (C) Copyright IBM Corp. 1995,2024                                         */
/* All Rights Reserved                                                       */
/*                                                                           */
/* US Government Users Restricted Rights -Use, duplication or                */
/* disclosure restricted by GSA ADP Schedule Contract with IBM Corp.         */
/*                                                                           */
/* IBM_PROLOG_END_TAG                                                        */
/* %Z%%M%	%I%  %W% %G% %U% */
/* Defines error codes that may be returned by the SHARK system         */
/* Sam Drake, IBM Almaden Research Center.                              */
/*                                                                      */

#ifndef _h_ShErrno
#define _h_ShErrno

#if !defined(GPFS_SOLARIS) || !defined(_KERNEL)
#include <errno.h>
#else
/* In Solaris kernel space, /usr/include/errno.h is just death */
#include <sys/errno.h>
#endif

#if defined(GPFS_SOLARIS)
#ifdef __cplusplus
extern "C" {
#endif
#endif  /* GPFS_SOLARIS */

/* verbs.h references errno and a few errno values, so include it here
   before errno is redefined and errno values are undefined below   */
#ifdef GDS
#if !defined(_KERNEL)      // verbs.h is not used to build kernel code
#include <infiniband/verbs.h>
#endif
#endif

typedef int IntRC;

/* Stand-alone utilities are not allowed to use the GPFS error numbers */
#if defined(STANDALONE_UTILITY) && !defined(DONT_UNDEF_ERRNO_NUMBERS)

typedef enum { E_OK = 0, E_EOF = 139 } Errno;

#else

/* GPFS error number definitions

   When adding new definitions, be sure to update the GPFSToSystemErrno()
   error number remapping function in sherrno.C.
*/
typedef enum
{
  E_FAILED =          -1,

  /* The first set of numbers match the AIX definitions */
  E_OK =               0,   /* No Trouble Found                               */
  E_PERM =             1,   /* Operation not permitted                        */
  E_NOENT =            2,   /* No such file or directory                      */
  E_SRCH =             3,   /* No such process                                */
  E_INTR =             4,   /* interrupted system call                        */
  E_IO =               5,   /* I/O error                                      */
  E_NXIO =             6,   /* No such device or address                      */
  E_2BIG =             7,   /* Arg list too long                              */
  E_NOEXEC =           8,   /* Exec format error                              */
  E_BADF =             9,   /* Bad file descriptor                            */
  E_CHILD =           10,   /* No child processes                             */
  E_AGAIN =           11,   /* Resource temporarily unavailable               */
  E_NOMEM =           12,   /* Not enough space                               */
  E_ACCES =           13,   /* Permission denied                              */
  E_FAULT =           14,   /* Bad address                                    */
  E_NOTBLK =          15,   /* Block device required                          */
  E_BUSY =            16,   /* Resource busy                                  */
  E_EXIST =           17,   /* File exists                                    */
  E_XDEV =            18,   /* Improper link                                  */
  E_NODEV =           19,   /* No such device                                 */
  E_NOTDIR =          20,   /* Not a directory                                */
  E_ISDIR =           21,   /* Is a directory                                 */
  E_INVAL =           22,   /* Invalid argument                               */
  E_NFILE =           23,   /* Too many open files in system                  */
  E_MFILE =           24,   /* Too many open files                            */
  E_NOTTY =           25,   /* Inappropriate I/O control operation            */
  E_TXTBSY =          26,   /* Text file busy                                 */
  E_FBIG =            27,   /* File too large                                 */
  E_NOSPC =           28,   /* No space left on device                        */
  E_SPIPE =           29,   /* Invalid seek                                   */
  E_ROFS =            30,   /* Read only file system                          */
  E_MLINK =           31,   /* Too many links                                 */
  E_PIPE =            32,   /* Broken pipe                                    */
  E_DOM =             33,   /* Domain error within math function              */
  E_RANGE =           34,   /* Result too large                               */
  E_NOMSG =           35,   /* No message of desired type                     */
  E_IDRM =            36,   /* Identifier removed                             */
  E_CHRNG =           37,   /* Channel number out of range                    */
  E_L2NSYNC =         38,   /* Level 2 not synchronized                       */
  E_L3HLT =           39,   /* Level 3 halted                                 */
  E_L3RST =           40,   /* Level 3 reset                                  */
  E_LNRNG =           41,   /* Link number out of range                       */
  E_UNATCH =          42,   /* Protocol driver not attached                   */
  E_NOCSI =           43,   /* No CSI structure available                     */
  E_L2HLT =           44,   /* Level 2 halted                                 */
  E_DEADLK =          45,   /* Resource deadlock avoided                      */
  E_NOTREADY =        46,   /* Device not ready                               */
  E_WRPROTECT =       47,   /* Write-protected media                          */
  E_FORMAT =          48,   /* Unformatted media                              */
  E_NOLCK =           49,   /* No locks available                             */
  E_NOCONNECT =       50,   /* no connection                                  */
  E_STALE =           52,   /* no filesystem                                  */
  E_DIST =            53,   /* old, currently unused AIX errno                */
  E_WOULDBLOCK =      54,   /* Operation would block                          */
  E_INPROGRESS =      55,   /* Operation now in progress                      */
  E_ALREADY =         56,   /* Operation already in progress                  */
  E_NOTSOCK =         57,   /* Socket operation on non-socket                 */
  E_DESTADDRREQ =     58,   /* Destination address required                   */
  E_DESTADDREQ =      58,   /* Destination address required                   */
  E_MSGSIZE =         59,   /* Message too long                               */
  E_PROTOTYPE =       60,   /* Protocol wrong type for socket                 */
  E_NOPROTOOPT =      61,   /* Protocol not available                         */
  E_PROTONOSUPPORT =  62,   /* Protocol not supported                         */
  E_SOCKTNOSUPPORT =  63,   /* Socket type not supported                      */
  E_OPNOTSUPP =       64,   /* Operation not supported on socket              */
  E_PFNOSUPPORT =     65,   /* Protocol family not supported                  */
  E_AFNOSUPPORT =     66,   /* Address family not supported by protocol family  */
  E_ADDRINUSE =       67,   /* Address already in use                         */
  E_ADDRNOTAVAIL =    68,   /* Can't assign requested address                 */
  E_NETDOWN =         69,   /* Network is down                                */
  E_NETUNREACH =      70,   /* Network is unreachable                         */
  E_NETRESET =        71,   /* Network dropped connection on reset            */
  E_CONNABORTED =     72,   /* Software caused connection abort               */
  E_CONNRESET =       73,   /* Connection reset by peer                       */
  E_NOBUFS =          74,   /* No buffer space available                      */
  E_ISCONN =          75,   /* Socket is already connected                    */
  E_NOTCONN =         76,   /* Socket is not connected                        */
  E_SHUTDOWN =        77,   /* Can't send after socket shutdown               */
  E_TIMEDOUT =        78,   /* Connection timed out                           */
  E_CONNREFUSED =     79,   /* Connection refused                             */
  E_HOSTDOWN =        80,   /* Host is down                                   */
  E_HOSTUNREACH =     81,   /* No route to host                               */
  E_RESTART =         82,   /* restart the system call                        */
  E_CLONEME =         82,   /* this is the way we clone a stream ...          */
  E_PROCLIM =         83,   /* Too many processes                             */
  E_USERS =           84,   /* Too many users                                 */
  E_LOOP =            85,   /* Too many levels of symbolic links              */
  E_NAMETOOLONG =     86,   /* File name too long                             */
  E_NOTEMPTY =        87,   /* Directory not empty                            */
  E_DQUOT =           88,   /* Disc quota exceeded                            */
  E_CORRUPT =         89,   /* Invalid file system control data               */
  E_REMOTE =          93,   /* Item is not local to host                      */
  E_NOSYS =          109,   /* Function not implemented  POSIX                */
  E_MEDIA =          110,   /* media surface error                            */
  E_SOFT =           111,   /* I/O completed, but needs relocation            */
  E_NOATTR =         112,   /* no attribute found                             */
  E_SAD =            113,   /* security authentication denied                 */
  E_NOTRUST =        114,   /* not a trusted program                          */
  E_TOOMANYREFS =    115,   /* Too many references: can't splice              */
  E_ILSEQ =          116,   /* Invalid wide character                         */
  E_CANCELED =       117,   /* asynchronous i/o cancelled                     */
  E_NOSR =           118,   /* temp out of streams resources                  */
  E_TIME =           119,   /* I_STR ioctl timed out                          */
  E_BADMSG =         120,   /* wrong message type at stream head              */
  E_PROTO =          121,   /* STREAMS protocol error                         */
  E_NODATA =         122,   /* no message ready at stream head                */
  E_NOSTR =          123,   /* fd is not a stream                             */
  E_NOTSUP =         124,   /* POSIX threads unsupported value                */
  E_MULTIHOP =       125,   /* multihop is not allowed                        */
  E_NOLINK =         126,   /* the link has been severed                      */
  E_OVERFLOW =       127,   /* value too large to be stored in data type      */

  E_EOF =            139,   /* Denied write past EOF (unpublished AIX value)  */

  /* Mappings for h_errno values (e.g. returned from gethostbyname) */

  E_HOST_FAILED =    170,   /* Unrecognized failure during lookup */
  E_HOST_NOT_FOUND = 171,   /* Authoritative Answer Host not found */
  E_TRY_AGAIN =      172,   /* Non-Authoritative Host not found, or SERVERFAIL */
  E_NO_RECOVERY =    173,   /* Non recoverable errors, FORMERR, REFUSED, NOTIMP */
  E_NO_DATA =        174,   /* Valid name, no data record of requested type */


  E_FIRST_GPFS_ERR =           180, /* All GPFS-specific error codes are >= 180 */

  /* Note that the gpfs library uses errno values in the 185-200 range.
     See gpfs.h for definitions. */

  E_INVAL_INUM =               185, /* invalid inode number */

  E_INVAL_FSSNAPID =           190, /* invalid fssnap id */
  E_INVAL_ISCAN =              191, /* invalid iscan pointer */
  E_INVAL_IFILE =              192, /* invalid ifile pointer */
  E_INVAL_IATTR =              193, /* invalid iattr structure */
  E_INVAL_RESTORE =            194, /* invalid restore pointer */
  E_INVAL_FSSNAPHANDLE =       195, /* invalid fssnap handle */
  E_INVAL_SNAPNAME =           196, /* invalid snapshot name */
  E_FS_NOT_RESTORABLE =        197, /* FS is not clean */
  E_RESTORE_NOT_ENABLED =      198, /* Restore was not enabled */
  E_RESTORE_STARTED =          199, /* Restore is running */
  E_INVAL_XATTR =              200, /* invalid extended attribute pointer */
  E_ISLNK =                    201, /* symlink is not supported by this operation */


  /* Error codes past this line are unique to GPFS.  When new entries are
     added, the mapping of GPFS error codes to local system error codes in
     sherrno.C must also be updated.  Note that AIX restricts the errno value
     to 8 bits! */

  E_NEED_FETCH =               201, /* Failed to grant lock in vfs due to insufficient
                                       token or invalid object */
  E_BADSECTORSIZE =            202, /* Physical sector size wrong */
  E_DEVNOTSUPPORT =            203, /* Device type not supported */
  E_NOBUFFERS =                204, /* Not enough buffer space for prefetch or pool
                                       size change. */
  E_OBJ_INVALID =              205, /* Lock request with LO_VALIDONLY failed because
                                       the object is not valid. */
  E_QUO_DONT_COUNT =           206, /* Do not count quotas for th given file. */
  E_MIXED_RS =                 207, /* Cannot mix restricted with other mounts */
  E_OFFLINE =                  208, /* Operation failed because a disk is offline */
  E_NOT_WRITTEN =              209, /* An attempt to write to a disk failed before any
                                       I/O was started, i.e., it is known that no data
                                       was written. */
  E_PROTECTED =                210, /* Disk writes into sector 0 of a disk device are
                                       not allowed */
  E_QUIESCED =                 211, /* Cannot do I/O to a disk after it has been
                                       quiesced as part of deleting it */
  E_NO_MGR =                   212, /* The current stripe group manager failed and no
                                       new manager could be appointed */
  E_WOULDWRAP =                213, /* LogFile::append would have had to wait for log
                                       wraparound processing */
  E_VALIDATE =                 214, /* Invalid checksum or other consistency check
                                       failure on disk data structure */
  E_CONFIG =                   215, /* Configuration invalid or inconsistent between
                                       different nodes */
  E_USAGE =                    216, /* Invalid parameters passed in to commands */
  E_DISK_UNAVAIL =             217, /* A replicated write did not complete on all
                                       disks, because one or more of the disks to be
                                       written is marked as unavailable in the stripe
                                       group descriptor.  However, at least one replica
                                       is still availables, and all available replicas
                                       were written sucessfully. */
  E_ALL_UNAVAIL =              218, /* A replicated read or write failed because none
                                       of the replicas were available. */
  E_NOREPLGRP =                219, /* Not enough replicas could be created because
                                       the desired degree of replication is larger than
                                       the number of failure groups. */
  E_NOREPLSPC =                220, /* Not enough replicas could be created because
                                       there was not enough space left in one of the
                                       failure groups. */
  E_NOBALSPC =                 221, /* There was not enough space left on one of the
                                       disks to properly balance the file according to
                                       the current stripe method. */
  E_NOBALAVAIL =               222, /* The file could not be properly balanced
                                       because one or more disks are unavailable. */
  E_CONFIGCHANGE =             223, /* Operation (e.g., allocation) did not complete
                                       because the stripe group configuration has
                                       changed. */
  E_NOLOG =                    224, /* No/not enough log files found. */
  E_RECOVERY =                 225, /* Log recovery failed. */
  E_ADDR_BROKEN =              226, /* All replicas were on disks that have since been
                                       deleted from the stripe group. */
  E_SG_BROKEN =                227, /* All replicas of some file system metadata were
                                       on disks that have since been deleted from the
                                       stripe group */
  E_INODE0_UNAVAIL=            228, /* Unable to find/read inode 0 - fatal error */
  E_SPARE_UNAVAIL=             229, /* While copying the log to a spare log file during
                                       log migrate the disk containing the spare became
                                       unavailable. */
  E_NEEDS_REPAIR =             230, /* Needs to run tsfsk to repair stripe group */
  E_NOT_MANAGER =              231, /* A message intended for the stripe group mananger
                                       or configuration manager, was received by a node
                                       that is (no longer) sg/cfg manager. */
  E_COMMVER_MISMATCH =         232, /* RPC not sent because destination node had
                                       incompatible communication version */
  E_NODE_FAILED =              233, /* An RPC failed because the sender was notified by
                                       the config mgr that the RPC destination failed */
  E_RECOV_INCOMPLETE =         234, /* Temporary inconsistency in directory block due
                                       to unfinished log recovery. */
  E_NOT_METANODE =             235, /* A message intended for the metanode was received
                                       by a node that is no longer the metanode for that
                                       file */
  E_FORMAT_INCOMPAT =          236, /* Incompatible file system format version found in
                                       the stripe group descriptor. */
  E_NO_QUOTA_INST =            237, /* Quota management is not installed for this SG */
  E_HOLE =                     238, /* Hole in file */
  E_NO_TOKEN =                 239, /* Insufficient token to grant lock in vfs */
  E_MULTI_RANGE_LOCK =         240, /* Attempt to lock more than one byte-range within
                                       the same file (not allowed due to limitation in
                                       TM, which could cause deadlock) */
  E_QUOTA_INIT =               241, /* Quota initialization/start-up failed */
  E_2SMALL =                   242, /* number given is out of allowed range */
  E_BREAK_OPLOCK =             243, /* An oplock must be broken before the operation
                                       can be processed. */
  E_BAD_POOL =                 244, /* Invalid storage pool */
  E_NO_QMGR =                  245, /* Quota management is installed but there is no
                                       quota manager */
  E_ALREADY_CONVERTED =        246, /* Attempting to convert/migrate a file system which
                                       is already at the correct level. */
  E_NNODE =                    247, /* Too many nodes for this operation */
  E_ZOMBIE_MSG =               248, /* Error code returned by message handlers to
                                       indicate that the message was not processed
                                       because the sender has failed or the stripe
                                       group to which the message applies was
                                       panicked on the node that sent the message.*/
  E_INVALIDATE_FAIL =          249, /* Unable to invalide a disk on deletion */
  // E_NOT_DATA_SHIPPING =     250, /* obsolete */
  E_DM_ABORT =                 251, /* File op is aborted because a DM event was
                                       responded with an abort by the DM application */
  E_ALLOC_TOO_HARD =           252, /* The effort needed to allocate a block exceeds
                                       the amount of effort that is allowed.  There
                                       may still be space, but it has not yet been
                                       located. */
  E_FS_DELETED =               253, /* The stripe group manager has resigned because
                                       it just deleted the file system (tsdefs command
                                       just finished). */
  E_USTORE_CRFS_FAIL =         254, /* Unable to create a UStore file system because
                                       more than one performance pool is specified,
                                       or no reliable data pool, or shared failure
                                       groups between a reliable pool and the
                                       performance pool. */

  E_MAX_ERRNO =                255, /* Maximum errno allowed to be returned from
                                       a kernel call under AIX. Return codes which
                                       are not mapped in the SharkToAIXErrno
                                       routine are set to this reserved value */
  E_RETRY_FSYNC =              256, /* Tells a waiting thread to retry its fsync
                                       because the threads that it was depending on
                                       to do the fsync on its behalf got errors
                                       before they could perform the fsync */
  E_SUSPEND_FLUSHER =          257, /* Tells Vdisk Flusher to suspend flushing. */
  E_SUSPEND_IM_ON_DA =         258, /* Tells Vdisk IM to suspend processing DA. */

  E_NOT_TM_SERVER =            301, /* This is not the token manager server node */
  E_REVOKE_PENDING =           302, /* A revoke for this token is already pending */
  E_NO_REPLY =                 303, /* No reply sent because none expected */
  E_CONN_BROKE =               304, /* Attempt to send a message failed even though
                                       the current status of destination node is 'up';
                                       waiting for the config manager to notify us that
                                       the node has failed. */
  E_NO_CLEANERS =              305, /* Too many cleaner threads running; could not
                                       become a prefetch thread */
  E_DELAYED_REPLY =            306, /* Reply will be sent later */
  E_DAEMON_DEATH =             307, /* Daemon shutting down */
  E_COULD_DEADLOCK =           308, /* Internal: when steal should not try harder */
  E_MOUNTED =                  309, /* File system is mounted. Used by commands,
                                       which need a file system unmounted. */
  E_NEEDS_COPIED =             310, /* Internal: data block needs to be copied to a
                                       snapshot prior to it being written */
  E_NOT_NSD_SERVER =           311, /* NSD I/O request was sent to a node which is
                                       no longer a server for the disk */
  E_NEEDS_RESTORE =            312, /* File system needs to be restored before
                                       it can be mounted. */
  E_NEEDS_DELSNAP =            313, /* Prior snapshot must be deleted before new
                                       delete/restore snapshot can execute. */
  E_IS_SNAPSHOT =              314, /* vfs operations are not allowed on snapshot
                                       files or directories */
  E_WRONG_CLUSTER =            315, /* message contains wrong or unexpected cluster id
                                       (e.g., msg to sgmgr, or nsd server with cluster
                                       id different from the cluster that the sgmgr
                                       or nsd server belongs to) */
  E_NOT_SNAPSHOT =             316, /* operation not allowed on non-snapshot file */
  E_SANERGY_SETUP =            317, /* Error SANergy MDC setup */
  E_SANERGY_LOCK  =            318, /* SANergy locks in effect */
  E_INVAL_DITTO =              319, /* Invalid ditto address found */
  E_ALLOC_SHIPPING =           320, /* alloc shipping is on */
  E_DUP_CONN =                 321, /* duplicate (cross) connection */
  E_CONFIG_FSBS =              323, /* Parameter incompatibility.  File system block
                                       size is larger than maxblocksize parameter */
  E_NO_WORKER_RESOURCES =      324, /* a MultiThreadWork work thread was unable
                                       to complete its step due to lack of resources */
  E_CCMGR_MIGRATE =            325, /* Disk lease could not be granted, because cfgmgr
                                       migrate is pending (old cfgmgr has resigned,
                                       but new one hasn't completed takover yet). */
  E_FGDL_REFUSED =             326, /* A fine-grain dir lock token was
                                       requested, but TM granted a normal token
                                       instead.  Or an FGDL RPC was invoked on
                                       a directory that cannot support FGDL. */
  E_COND_LIST_FULL =           327, /* Cond var list in event word is full */
  E_INVAL_FSETSNAPHANDLE =     328, /* invalid fsetsnap handle */
  E_INVAL_FSETNAME =           329, /* invalid fsetsnap handle */
  E_NO_MORE_WORK =             330, /* a MultiThreadWork work thread determined
                                       that there is no more work to do */
  E_CHECKSUM_RETRY =           331, /* Checksum error encountered, possibly due
                                       data being updated while being read;
                                       caller should retry the read */
  E_QUIESCE_TIMEDOUT =         332, /* Quiescing filesystem took too long */
  E_LOG_HOLE =                 333, /* Log recovery ended without processing all
                                       committed log records */
  E_CONN_DROP =                334, /* Error encountered during RPC processing
                                       that requires quiet shutdown of a
                                       connection */
  E_EXCL_IND_ACC_LOCK_NEEDED = 335, /* Exclusive indAccessLock needed */
  E_NEED_FLUSH_FLAG          = 336, /* Retry after acquiring the flush flag */
  E_REFUSE_APPEND_SHIP       = 337, /* Metanode Refused the append ship request */

  /* Error code returned by token manager when it receives a wa token from
   * another node with CTM_A_WILLING_TO_FUNCTION_SHIP token flag and
   * the token manager then wants the node to ship append to metanode */
  E_SHIP_TO_MN               = 338, /* Ship append to MN */
  E_APPEND_SHIP_OFF          = 339, /* Metanode is not in the append ship mode */
  E_INFO_APPEND_SHIP_ACTIVE  = 340, /* The TM was in the append ship mode right
                                       before the MN change. */
  E_REPLICA_MISMATCH         = 341, /* Error returned by online replica
                                       compare when run with internal option
                                       '--return-err-on-mismatch' and replica
                                       mismatches are found */
  E_REFUSE_DATA_SHIP         = 342, /* Data ship request is refused */
  E_ABORT_DATA_SHIP          = 343, /* Data ship operations are aborted */

  /* Licensing : you must also update sherrno.C */
  E_NO_EXTLIC =                400,

  /* When adding new error codes, you must also update sherrno.C */

  E_PANIC =                    666, /* Unrecoverable error encountered */
  E_PANIC_MSG =                667, /* Panic message received from another node */
  E_NO_QUORUM =                668, /* TS daemon lost the quorum. Shutdown
                                       the daemon */
  E_JOIN_FAIL =                669, /* TS daemon failed to join the group */
  E_ADAPTER_FAIL =             670, /* TS daemon failed because of adapter
                                       failure */
  E_RVSD_NOT_ACTIVE =          671, /* rvsd is not active */
  E_VSD_FENCE_FAILED =         672, /* rvsd fence/unfence request failed */
  E_DAEMON_INCOMPATIBLE =      673, /* mmfsd daemon is incompatible */
  E_TOKEN_BUSY =               674, /* Token is still in use by DFS */
  E_DM_INVALID_TOKEN =         675, /* Invalid DMAPI event token */
  E_DM_INVALID_EVTYPE=         676, /* Invalid DMAPI event type */
  E_DM_INVALID_NODEID=         677, /* Invalid DMAPI nodeid */
  E_DM_TIMEOUT       =         678, /* Timeout tick occurred */
  E_QMGR_NOT_READY =           679, /* After recovery quota mgr is not ready */
  E_ALREADY_EXPORTED =         680, /* Another node already exports via DFS */
  E_DM_FILESIZE =              681, /* filesize changed between event and vnop */
  E_DM_NODISP =                682, /* No disposition for dmapi async event */
  E_DM_FORCEDASYNC =           683, /* Event continues asynch and vnop does not wait */
  E_DM_FAILED =                684, /* Event processing failed at session node */
  E_DM_NOTSENT =               685, /* Event not sent to session node */
  E_DM_UNKNOWNERR =            686, /* Unknown error returned from DMAPI call */
  E_DM_NAMESPC_DISP =          687, /* No disposition for namespace event */
  E_DM_DATA_DISP =             688, /* No disposition for dmapi data event */
  E_DM_NOSPC_DISP =            689, /* No disposition for dmapi nospace event */
  E_DM_UNMNT_DISP =            690, /* No disposition for dmapi pre/unmount event */
  E_DM_MOUNT_DISP =            691, /* No disposition for dmapi mount event */
  E_NO_PROGRESS =              692, /* DFS Revoke seems to be stalled, either */
                                    /* due to network partition or deadlock   */
  E_CONN_FATAL =               693, /* RPC failed because the destination node refused
                                       the connection.  Retrying is not likely to
                                       succeed. */
  E_CONN_BAD_ADDR =            694, /* Connection failed due to bad_addr reply */
  E_DISK_FENCED =              695, /* Disks fenced from the local node in
                                       useSingleNodeQuorum  mode */
  E_DIO_FAILED =               696, /* Direct I/O operation failed */
  E_DIO_RETRY =                697, /* Direct I/O operation must be retried */
  E_CONN_ZOMBIE =              698, /* Connection failed due to incorrect GroupId */
  E_CONN_LOST_QUORUM =         699, /* Connection refused because quorum loss
                                       shutdown is still in progress. */
  E_PRIVILEGE_NOT_HELD    =    701, /* Windows privilege not held */
  E_GET_CLIENT_CREDS_FAIL =    702, /* spsec_get_client_creds() failed */
  E_AUTHENTICATION_FAILED =    703, /* an authentication oper has failed */
  E_BAD_CREDENTIALS =          704,
  E_BAD_FORMAT =               705,
  E_BAD_UUID =                 706,
  E_DISK_FENCE_FAILED =        707,
  E_ALLOW_SHARE_DELETE =       708, /* Lock conflict but ALLOW_SHARE_DELETE on */
  E_ACLVERSION =               709, /* ACL version unsupported */
  E_BAD_EDITOR =               710, /* Invalid or missing EDITOR env variable */

  E_SAN_PERM =                 711, /* SANergy operations not allowed on
                                       this file */
  E_BAD_REPLICA =              712, /* Validation error in a replica */
  E_SAN_PARTIAL =              713, /* SANergy: not enough space provided
                                       for MDC to report back all info */
  E_NO_DISKLEASE =             714, /* no disk lease */
  E_PROPAGATE =                715, /* mmfsd could not propagate config value */
  E_SAN_NOTLOCKED =            716, /* File is not sanergy locked */
  E_CONN_UNKNOWN_ADDR =        717, /* Connection failed due to unknown_addr */
  E_CONN_NOTREADY =            718, /* Connection failed because destination
                                       is still processing previous node
                                       failure. */
  E_CONN_UNKNOWN_CLUSTER =     719, /* Connection failed due to unknown_cluster */
  E_SECURITY_CFG =             720, /* Security configuration data is
                                       inconsistent or unavailable. */
  E_SECURITY_LIB =             721, /* Failed to load or initialize security
                                       library. */
  E_SECURITY_CTX =             722, /* Failed to create or initialize a
                                       security context. */
  E_WANT_WRITE =               723, /* Non blocking SSL connection would block
                                       because TCP socket is not writable. */
  E_WANT_READ =                724, /* Non blocking SSL connection would block
                                       because TCP socket is not readable. */
  E_FS_RENAMED =               725, /* The stripe group manager has resigned
                                       because it just renamed the file system
                                       (tschfs command just finished). */
  E_AUTHORIZATION_FAILED =     726, /* Authorization failed. */
  E_SECURITY_CONN =            727, /* Failed to create a secure connection. */
  E_SOME_FAILED =              728, /* Indicates partial failures */
  E_CLOSE_FAILED =             729, /* The secure connection was not cleanly */
  E_IS_FILESET =               730, /* vfs rmdir is not allowed on filesets */
                                    /*  + llMsgCopyInodeBlock passed FMF ino */
  E_UNKNOWN_CLUSTER_NAME =     731, /* Cluster name was not known. */
  E_EXTRANEOUS =               732, /* Extraneous data received */
  E_RDMA =                     733, /* RDMA on server encountered an error */
  E_NODE_FAILED_DSENT =        734, /* send error after data sent to srvr */
  E_RDMA_VERBS =               735, /* server enabled for VERBS RDMA */
  E_RDMA_UDAPL =               736, /* server enabled for uDAPL RDMA */
  E_RDMA_NOT_ENABLED =         737, /* RDMA is not enabled */
  E_RDMA_NO_PORTS =            738, /* no ports defined for RDMA */
  E_RDMA_LOCAL =               739, /* RDMA connection error on local node */
  E_RDMA_USE_TCP =             740, /* connection should use TCP and not RDMA */
  E_RDMA_NOT_SUPPORTED =       741, /* RDMA connection not supported */

  E_MUST_SPLIT =               742, /* FGDL metanode must split dir block */
  E_DONTFREE =                 743, /* Unused */
  E_RECONNECT =                744, /* Automatic reconnection is in progress */
  E_REPLY_LOST =               745, /* RPC reply was lost; resend original msg */
  E_DM_MANAGED =               746, /* Request to DM managed region */
  E_OBJ_DEST =                 747, /* EE Object destroyed while processing */
  E_NODE_EXPELLED =            748, /* Node has been expelled from the cluster
                                       by the administrator */
  E_IOCBQUEUED =               749, /* Linux AIO has been queued */
  E_PG_NOT_RESTORABLE =        750, /* A vdisk partition group has too many
                                       damaged partitions to reconstruct data. */
  E_NOMDTHRESH =               751, /* Could not find more than the threshold
                                       number of vdisk metadata slots, so the
                                       metadata block was not rewritten */
  E_SENSE_DATA =               752, /* Sense data from a scsi cmd */
  E_NOT_MOUNTED =              753, /* File system is unmounted. Used by commands
                                       which need a file system mounted. */

  E_DIO_DD_PSEU =              754, /* Direct IO failed due to device being
                                       a psuedo device or nsd device. In this
                                       case mmap writes call daemon for IO
                                       with buffer lock */
  E_CACHE_CONFLICT =           755, /* Cache file and original are out of
                                       sync with each other.  Handling is
                                       dependent on the current conflict
                                       semantics. */
  E_RG_PANIC =                 756, /* Vdisk recovery group panic: Too many disks
                                       unavailable to continue serving this RG,
                                       this server will resign, hopefully failover
                                       to an alternate server will help. */
  E_DM_CDITTO =                757, /* DMAPI invis write to a CDITTO disk address */
  E_EXPIRED =                  758, /* Panache fileset data has expired */
  E_RG_PGMAP_FULL =            759, /* Vdisk recovery group  PG map has no
                                       free entries. */
  E_CDITTO_LOCK =              760, /* Inode lock order violation when resolving CDITTO */
  E_ISCDITTO =                 761, /* Attempt to read CDITTO block when skip was requested
                                       or CDITTO found in higher-level indirect block */
  E_NOTCDITTO =                762, /* Clone fetch request found non-cditto disk address */
  E_RPC_DATA_CHECKSUM =        763, /* checksum error detected during RPC
                                       data transfer */
  E_LOG_SPLICE =               764, /* incomplete log record due to
                                       vdisk recovery group failure */
  E_REQUEUED =                 765, /* pcache msg was left in requeued state */
  E_UNLINK_CHECK =             766, /* app node should check if it can unlink inode */
  E_RDMA_DUP_CONN =            767, /* RDMA connection is already established or in progress */
  E_RDMA_RDMACM =              768, /* server enabled for RDMA CM */
  E_PARENT_CDITTO =            769, /* clone parent of this file has a CDITTO here */
  E_CDITTO_DANGLING =          770, /* CDITTO points to a non-existant file */
  E_CACHE_NOT_ACTIVE =         771, /* panache fileset is not in active state */
  E_WAITERS =                  772, /* back out of sync if there are XW waiters */
  E_QUEUE_PSNAP =              773, /* Queue is in psnap mode */
  E_PRETRANSLATE =             774, /* a pre-translated pagepool cannot have size decreased */
  E_RDMA_VERBS_SCATTER =       775, /* Server enabled for scatter buffers */
  E_RDMA_RDMACM_SCATTER =      776, /* Server enabled for RDMACM and scatter buffers */
  E_DM_RETRY =                 777, /* retry dm get events */
  E_ENC_IKM_NOAUTH =           778, /* Encryption: Unable to retrieve key */
  E_ENC_IKM_OTHER =            779, /* Encryption: Other errors produced by
                                       the IKM */
  E_ENC_GCRYPTO_INIT =         780, /* Encryption: error in gcrypto -
                                       gcryptoCipherInitCtx() */
  E_ENC_GCRYPTO_ENCDEC =       781, /* Encryption: error in gcrypto -
                                       encrypt/decrypt */
  E_ENC_GCRYPTO_OTHER =        782, /* Encryption: error in gcrypto - other */
  E_ENC_KEY_MARSHAL =          783, /* Encryption: error in marshalling key */
  E_ENC_KEY_UNMARSHAL =        784, /* Encryption: error in unmarshalling key */
  E_ENC_INTERNAL =             785, /* Encryption: data structure error */
  E_ENC_CTX_NOT_READY =        786, /* Encryption: encryption context
                                       not ready */
  E_DEFAULT_QUOTA_OFF   =      787, /* default quota for a fileset is disabled */
  E_HOSPITAL_PTN =             788, /* Hospital test pattern found instead of valid data. */
  E_RESTRATEGIZE =             789, /* GNR AU log writer needs to use a
                                       different strategy */
  E_ALL_ZEROS_BLOCK   =        790, /* An all-zero block is detected and can be deallocated */
  E_SYNC_WRITE_ONLY  =         791, /* Need to do fsync due to low space condition */
  E_RECLAIM_DISK_SPACE   =     792, /* Encountered error when reclaiming thin-provisioned disk space */
  E_TAKEOVER_INITIATED  =      793, /* Encountered error when sgmount is called
                                       while takeover is just started and
                                       sg_mgr_init is not called yet */
  E_RDMA_TEST =                794, /* Inject error in verbsClientEnd_i */
  E_DIO_4K_ALIGNED =           795, /* Non 4k aligned DIO request on 4k native disk */
  E_COMPRESSED =               796, /* data block is compressed when trying to access uncompressed data */
  E_UNCOMPRESSED =             797, /* data block is uncompressed when trying to access compressed data*/
  E_STALE_HANDLE =             798, /* An internal handle is no longer valid. */
  E_COMPRESS_HYPERALLOC =      799, /* Compression of a file in hyperalloc mode not allowed. */

  E_CCR_OK =                  E_OK, /*  */
  E_CCR_MAX_RETRIES =          801, /* Max no of retries exceeded */
  E_CCR_INIT_NODES =           802, /* Node initialization failed */
  E_CCR_INIT_DISKS =           803, /* Disk initialization failed */
  E_CCR_NAME_NOT_FOUND =       804, /* Name not found on get */
  E_CCR_STALE_VERSION =        805, /* Version mismatch on conditional put */
  E_CCR_UPTODATE_VERSION =     806, /* Version match on conditional get */
  E_CCR_INVALID_VERSION =      807, /* Invalid version on put */
  E_CCR_NOT_ACTIVE_SERVER =    808, /*  */
  E_CCR_NO_QUORUM =            809, /*  */
  E_CCR_IO_ERROR =             810, /*  */
  E_CCR_NO_DIR =               811, /*  */
  E_CCR_NO_NODELIST =          812, /* ccr.nodes file missing or empty */
  E_CCR_NO_FILE =              813, /*  */
  E_CCR_ALREADY_INIT =         814, /*  */
  E_CCR_NOT_LEADER =           815, /* Local node is no longer leader */
  E_CCR_STALE_SEQ =            816, /* Seq no changed during Synod */
  E_CCR_PR_FAILED =            817, /* Setting PR key seed failed */
  E_CCR_BAD_MSG =              818, /* Ill-formed message received */
  E_CCR_LEADER_UNAVAIL =       819, /* Unable to contact current leader */
  E_CCR_NOTIFY_FAILED =        820, /* Notifying other nodes about commits failed */
  E_CCR_CREATE_DIR =           821, /* Creation of CCR directory failed */
  E_CCR_NOT_AUTHORITATIVE =    822, /* Returned CCR state is best guess (not known to be up-to-date) */
  E_CCR_NO_CONNECT =           823, /* Unable to reach any quorum node (Check your firewall or network settings) */
  E_CCR_FILE_EMPTY =           824, /* File is empty */
  E_CCR_TIMEOUT =              825, /* Request ran into timeout */
  E_CCR_MIN_RELEASE_LEVEL =    826, /* Min. release level does not support request */
  E_CCR_DISK_VERIFICATION =    827, /* Tiebreaker disk verification failed */
  E_CCR_OLD_DAEMON_VERSION =   828, /* Older daemon version */
  E_CCR_STALE_FILE =           829, /* Stale CCR file */

  E_VIOREQ_INCOMPAT =          830, /* Incompatible VIORequest found */
  E_VIOREQ_COMPAT_HONOR_QUEUE = 831, /* Only compatible VIORequests found */
  E_VIOREQ_LOCK_DROPPED =      832, /* VIORequest list may have changed; rescan */
  E_GNRLOG_RECLASSIFY_IO =     833, /* deferred log IO needs immediate IO */
  E_BGD_FULL =                 834, /* deferred deletion list for recovery is
                                       full */
  E_ZONE_CROSSING =            835, /* GNR MD write attempted to cross a zone
                                       boundary */
  E_APPEND_WAIT =              836, /* GNR MD write would have to block due
                                       to a zone crossing already in progress */
  E_ENC_POLICY =               837, /* Encryption: a problem processing
                                       encryption policy rules */
  E_LOG_EOF =                  838, /* end of GNR log encountered */
  E_LOG_ZONE_PREFIX_HOLE =     839, /* higher ssn than expected in GNR
                                       log recovery */
  E_COMPRESS_NOT_ALLOWED =     840, /* File compression not allowed                   */
  E_WMRPC_NOT_SERVING_RG =     841, /* RPC response if RGMaster object is
                                       not currently being served. */
  E_WMRPC_WRONG_STATE =        842, /* RPC response if RGMaster object
                                       is not currently in the correct state */
  E_WMRPC_STALE_OBJECT =       843, /* RPC response if indicated LGMaster,
                                       DAMaster, PDMaster of VdiskMaster
                                       object does not exist in the recovery
                                       group descriptor. */
  E_SED_COMM_NOTINITIALIZED =  844, /* Class SedCommunication needed for
                                       forming TCP/IP packets to interact
                                       with the drive is not initialized. */
  E_SED_NODATA_RESPBUFFER =    845, /* After doing a SCSI Recv, there is no
                                       data in the response buffer received
                                       from the drive. */
  E_SED_OPENSESSION_FAILED =   846, /* Opening a ADMIN/LOCKING session with
                                       SED drive failed. */
  E_SED_CLOSESESSION_FAILED =  847, /* Closing an open ADMIN/LOCKING session
                                       with a SED Drive failed. */
  E_SED_SESS_NOTINITIALIZED =  848, /* Class SedSession needed for holding
                                       Session information is not
                                       initialized. */
  E_SED_INVALID_COMMPARAMS =   849, /* The members of class SedCommunication
                                       are not initialized. */
  E_SED_INVALID_SESSPARAMS =   850, /* The members of class SedSession are
                                       not initialized. */
  E_SED_INVALID_SCSIREQPARM =  851, /* The members of class SedSCSIRequest are
                                       not initialized. */
  E_SED_INVALID_COMMDATA =     852, /* The data buffer which is used to read/
                                       write data from/to the SED drive is
                                       not allocated and hence invalid. */
  E_SED_INVALOPTYPE =          853, /* The requested operation type is not
                                       supported by the SED drive. */
  E_SED_NODATA_INCOMMPACKET =  854, /* While creating Communication Packet for
                                       interacting with the SED drive, there
                                       was an error bcoz of which data could
                                       not be encoded in the packet. */
  E_SED_INVALID_SCSIREQ =      855, /* Class SedSCSIRequest needed for handling
                                       SCSI Requests is not initialized. */
  E_SED_DECODEMSID_FAILED =    856, /* After doing a SCSI Recv on the SED Drive
                                       on which we had sent a request for
                                       getting MSID, there is an issue with the
                                       response sent from the drive. Hence,
                                       MSID could not be decoded frm the data
                                       we received from the drive. */
  E_SED_STARTTRANS_FAILED =    857, /* Starting a transaction on a SED Drive
                                       failed. */
  E_SED_ENDTRANS_FAILED =      858, /* Ending a transaction on a SED Drive
                                       failed bcoz of which none of the
                                       operations embedded within the
                                       transaction got executed. */
  E_SED_OPFAILED =             859, /* The current operation on the SED Drive
                                       failed. */
  E_SED_INVALID_DRIVEKEY =     860, /* The Key/Pin needed to do an enroll or
                                       authentication on the drive is not
                                       set and hence invalid. This is different
                                       from Authentication Failure using a Key
                                    */
  E_SED_INVALPARAM =           861, /* An incorrect value for a parameter is
                                       set. */
  E_SED_AUTH_FAILED =          862, /* Authentication on a band using the
                                       supplied Key/PIN failed. */
  E_SED_UNSUPPORTED_BAND =     863, /* The band on which the operation is being
                                       attempted is not supported. */
  E_SED_GETMSID_FAILED =       864, /* Fetching MSID of the SED Drive failed. */
  E_SED_ISKLMGETKEY_FAILED =   865, /* Getting Key from the ISKLM server failed
                                    */
  E_SED_DECODEATTR_FAILED =    866, /*Fetching any of the attributes of Band
                                      RL/WL/RLE/WLE/start_LBA/Range failed. */
  E_TSPREPARENEWPDISKNOCHG =   867, /* The tspreparenewpdiskforuse script
                                       made no changes to the disk. */
  E_NOTSCSIDEVICE =            868, /* Attempt to execute SCSI command on a
                                       device that's not capable of that. */
  E_NO_ITEMS =                 869, /* Not enough items available to
                                       satisfy FreeItems::fiAlloc */
  E_CCR_CONNECT_FAILED =       870, /* Connection to CCR quorum node failed. */
  E_USER_DECOMPRESS =          871, /* partion needs to be decompressed by a user-space lib */
  E_KERNEL_DECOMPRESS =        872, /* partion needs to be decompressed in kernel */

  E_ITEM_OVERFLOW =            873, /* incrementing a field within ItemIndices
                                       would overflow */
  E_COMPRESS_LIB_SUPPORT =     874, /* compression library specified by user is not supported */
  E_GNRB_KEY_CONFLICT =        875, /* Proposed GNRBase object has a key
                                       conflict with an existing object. */
  E_GNR_REFRESHER_YIELD =      875, /* WorkerObjectRefresher asked to yield
                                       the thread. */
  E_GNR_REFRESHER_NESTED_WAIT = 876, /* WorkerObjectRefresher blocked on pdisk
                                       config version update. */
  E_CREATE_CQ_FAILED =         877,  /* ibv_create_cq failed */
  E_VIOREQ_COMPAT_SKIP_QUEUE = 879,  /* Only compatible VIORequests found
					which need not be queued behind
					other requests. */
  E_COMPRESS_LIB_LOAD_FAILED = 880, /* compression library failed to load */
  E_PARTIAL_SEC_SCANNED      = 881, /* Used by tsgnrlslsec command. If there
                                       is a partial sector in the current
                                       buffer while scanning for lostsectors,
                                       then we need to skip this sector. */
  E_XATTR_NO_FLUSH_FLAG =      882, /* acquireXAttrOverflow returns this error
                                      if it gets error from SFSCopyDataOnWrite.
                                      It indicates its caller it released
                                      the flush flag and no need to release
                                      it */
  E_CCR_NO_INIT =              883, /* CCR initialization failed or not done */
  E_CCR_SELECT_OTHER_SERVER =  884, /* Client must select another CCR server for
                                       its request. */
  E_PAGEFAULT_NO_BUF        =  885, /* Buffer that pagefault need is not
                                       valid. */
  E_PAGEFAULT_LOCK_SKIPPED =   886, /* Lock acquiring is skipped because a
                                       cxiUXfer is processing on the same
                                       object */
  E_MAINT_MODE =               888, /* File System is in maintenance mode */
  E_CCR_NO_MODIFICATIONS =     889, /* CCR patch Paxos command found no
                                       modifications */
  E_RG_PGLOOP_LIMIT =          890, /* PG loop would exceed MAX_PG_LOOP_DEPTH */
  E_REBUILD_DEFERRED =         891, /* Integrity Manager(IM) has temporarily
                                       deferred Rebuild. */
  E_CCR_NOT_A_QUORUM_NODE =    892, /* The CCR command must be executed on a
                                       quorum node. */

  E_VWCE =                     893, /* An operation failed due to a GNR drive
                                       having volatile write caching enabled.*/
  E_EVLOG_FLUSH_TIMEDOUT =     894, /* Event flush timed out */
  E_CCR_MAX_REQUESTS_REACHED = 895, /* max number of CCR requests reached */
  E_MCOT_FAILOVER =            896, /* Need to failover to another connection
                                       for sending */
  E_MCOT_FAILOVER_IF_NEEDED  = 897, /* Need to check if resend is needed
                                       before failover to another connection
                                       for resending */
  E_MCOT_DUP_ACK             = 898, /* Error code in an ACK message which
                                       indicates this is a dup ACK message */
  E_MCOT_EARLY_ACK           = 899, /* Error code in an ACK message which
                                       indicates this is an early ACK message */
  E_MCOT_NEG_ACK             = 900, /* Error code in an ACK message which
                                       indicates this is a negative ACK
                                       message, for future use */

  E_FSCKX_STALE              = 901, /* Fsckx request is for a different run. */
  E_FSCKX_NOREF              = 902, /* Failed to get fsckx object reference. */
  E_RDMA_QUEUE_FULL_USE_TCP  = 903, /* Failed to send RPC because verbs sending queue
                                       is full */
  E_FSCKX_ENDING             = 904, /* Fsckx run is ending. */

  E_WRONG_PDISK_STATE        = 905, /* A GNR pdisk I/O failed because it was
                                       issued to a disk whose given state
                                       makes it unavailable for the given
                                       request. */
  E_CCR_DBLK_SIZE_EXCEEDED   = 906, /* Failed to write Paxos file because
                                       maximum Paxos state size (CCR_DBLK_SIZE)
                                       exceeded. */
  E_CCR_STRING_TOO_LONG      = 907, /* Value commit failed because name or value
                                       length exceeds maximum length. */
  E_RDMA_INITIATOR_PORT_DOWN = 908, /* Initiator port for a DC RDMA operation is
                                       in down state. */
  E_NO_RDMA_PORT_FOUND       = 909, /* No matching RDMA port for the requested
                                       RDMA operation could be found. */
  E_LOOKUP_JUNCTION_RETRY    = 910, /* Retry fileset junction lookup */
  E_CCR_SUSPENDRESUME        = 911, /* invalid suspend/resume request based on
                                       current status of mmfsd/mmsdrserv */
  E_MROT_CONN_RETRY          = 912, /* Error code indicates the outbound connection
                                       needs to retry since the peer side doesn't
                                       support MROT. */
  E_CCR_SUSPENDED            = 913, /* CCR request rejected by suspended mmsdrserv */
  E_NOPHYSPC                 = 914, /* No physical space left on device. */
  E_REPLY_SENT               = 915, /* Reply has been already sent */
  E_DPP_ENABLED              = 916, /* Dynamic Pagepool is enabled */
#ifdef MITIGATE_RECALL_STORMS
  E_DM_NOTDITTO              = 917, /* DMAPI invis write to a non-DITTO DA in snapshot */
#endif
#ifdef TPM2_SUPPORT
  E_TPM_INIT                 = 918, /* TPM initialization failed*/
#endif
  E_TS_PREPAREPDISK_USEREXIT_FAILED = 919, /* One of the preparepdisk userexit script failed */
  E_CONTINUE                 = 999, /* The operation continues */

  /* Numbers greater than this are reserved for mapping local system error
     values. */
  E_MAX_GPFS_ERRNO =          1000,

  /* The following errors are defined by Linux (and possibly other platforms),
     but not AIX.  These are not currently used by GPFS code. */
  E_BADE =                    1052, /* Invalid exchange                               */
  E_BADR =                    1053, /* Invalid request descriptor                     */
  E_XFULL =                   1054, /* Exchange full                                  */
  E_NOANO =                   1055, /* No anode                                       */
  E_BADRQC =                  1056, /* Invalid request code                           */
  E_BADSLT =                  1057, /* Invalid slot                                   */
  E_BFONT =                   1059, /* Bad font file format                           */
  E_NONET =                   1064, /* Machine is not on the network                  */
  E_NOPKG =                   1065, /* Package not installed                          */
  E_ADV =                     1068, /* Advertise error                                */
  E_SRMNT =                   1069, /* Srmount error                                  */
  E_COMM =                    1070, /* Communication error on send                    */
  E_DOTDOT =                  1073, /* RFS specific error                             */
  E_NOTUNIQ =                 1076, /* Name not unique on network                     */
  E_BADFD =                   1077, /* File descriptor in bad state                   */
  E_REMCHG =                  1078, /* Remote address changed                         */
  E_LIBACC =                  1079, /* Can not access a needed shared library         */
  E_LIBBAD =                  1080, /* Accessing a corrupted shared library           */
  E_LIBSCN =                  1081, /* .lib section in a.out corrupted                */
  E_LIBMAX =                  1082, /* Attempting to link in too many shared libraries  */
  E_LIBEXEC =                 1083, /* Cannot exec a shared library directly          */
  E_STRPIPE =                 1086, /* Streams pipe error                             */
  E_UCLEAN =                  1117, /* Structure needs cleaning                       */
  E_NOTNAM =                  1118, /* Not a XENIX named type file                    */
  E_NAVAIL =                  1119, /* No XENIX semaphores available                  */
  E_ISNAM =                   1120, /* Is a named type file                           */
  E_REMOTEIO =                1121, /* Remote I/O error                               */
  E_NOMEDIUM =                1123, /* No medium found                                */
  E_MEDIUMTYPE =              1124, /* Wrong medium type                              */
  E_NOKEY =                   1126, /* Required key not available                     */
  E_KEYEXPIRED =              1127, /* Key has expired                                */
  E_KEYREVOKED =              1128, /* Key has been revoked                           */
  E_KEYREJECTED =             1129, /* Key was rejected by service                    */

  /* The following errors are defined by Windows Posix (SUA), but not AIX or Linux.
     These are not currently used by GPFS */
  E_FIRST_WINDOWS_ERRNO =     2000, /* Start of Windows error code range */
  E_BADENCRYPTDATA =          2063, /* Invalid encrypted data found */
  E_NOSETUID =                2066, /* Cannot exec() - setuid/setgid not enabled */
  E_FTYPE =                   2079, /* Inappropriate file type or format */
  E_WINPATH =                 2087, /* Can't map pathname to Windows namespace */
  E_2SMALL_WIN =              2089, /* user data buffer too small */
  E_HOSTNOTFOUND =            2090,
  E_TRYAGAIN =                2091,
  E_NORECOVERY =              2092,
  E_EMPTY =                   2093, /* Queue or list is empty */
  E_NEED_QUOTAPERFILESET =    2094, /* --perfileset-quota should be on */

  /* The following errors mark the beginning and end of a reserved range.
     In GPFSToSystemErrno(), when a GPFS Errno values does not have a system
     errno mapping and the value might collide with some arbitrary system
     errno, then the GPFS Errno is remapped into this range.
     (Windows only at present.) */
  E_FIRST_UNMAPPED_ERRNO =    8000,
  E_LAST_UNMAPPED_ERRNO =     E_FIRST_UNMAPPED_ERRNO + E_FIRST_GPFS_ERR,

  /* The following are LDAP Results codes offset by E_FIRST_LDAP_RESULTS.
     I expect these codes will be managed as a range rather than individual values,
     but have included the individual values here to allow an explicit mapping and
     aid in debugging. LDAP results are based on a standard; this particular list
     was started using the Windows winldap.h file. Values under 0x60 appear consistent
     between implementations. */
  E_FIRST_LDAP_RESULT               =   9000,
  E_LDAP_SUCCESS                    =   E_FIRST_LDAP_RESULT + 0x00,
  E_LDAP_OPERATIONS_ERROR           =   E_FIRST_LDAP_RESULT + 0x01,
  E_LDAP_PROTOCOL_ERROR             =   E_FIRST_LDAP_RESULT + 0x02,
  E_LDAP_TIMELIMIT_EXCEEDED         =   E_FIRST_LDAP_RESULT + 0x03,
  E_LDAP_SIZELIMIT_EXCEEDED         =   E_FIRST_LDAP_RESULT + 0x04,
  E_LDAP_COMPARE_FALSE              =   E_FIRST_LDAP_RESULT + 0x05,
  E_LDAP_COMPARE_TRUE               =   E_FIRST_LDAP_RESULT + 0x06,
  E_LDAP_AUTH_METHOD_NOT_SUPPORTED  =   E_FIRST_LDAP_RESULT + 0x07,
  E_LDAP_STRONG_AUTH_REQUIRED       =   E_FIRST_LDAP_RESULT + 0x08,
  E_LDAP_PARTIAL_RESULTS            =   E_FIRST_LDAP_RESULT + 0x09,
  E_LDAP_REFERRAL                   =   E_FIRST_LDAP_RESULT + 0x0a,
  E_LDAP_ADMIN_LIMIT_EXCEEDED       =   E_FIRST_LDAP_RESULT + 0x0b,
  E_LDAP_UNAVAILABLE_CRIT_EXTENSION =   E_FIRST_LDAP_RESULT + 0x0c,
  E_LDAP_CONFIDENTIALITY_REQUIRED   =   E_FIRST_LDAP_RESULT + 0x0d,
  E_LDAP_SASL_BIND_IN_PROGRESS      =   E_FIRST_LDAP_RESULT + 0x0e,
  E_LDAP_NO_SUCH_ATTRIBUTE          =   E_FIRST_LDAP_RESULT + 0x10,
  E_LDAP_UNDEFINED_TYPE             =   E_FIRST_LDAP_RESULT + 0x11,
  E_LDAP_INAPPROPRIATE_MATCHING     =   E_FIRST_LDAP_RESULT + 0x12,
  E_LDAP_CONSTRAINT_VIOLATION       =   E_FIRST_LDAP_RESULT + 0x13,
  E_LDAP_ATTRIBUTE_OR_VALUE_EXISTS  =   E_FIRST_LDAP_RESULT + 0x14,
  E_LDAP_INVALID_SYNTAX             =   E_FIRST_LDAP_RESULT + 0x15,
  E_LDAP_NO_SUCH_OBJECT             =   E_FIRST_LDAP_RESULT + 0x20,
  E_LDAP_ALIAS_PROBLEM              =   E_FIRST_LDAP_RESULT + 0x21,
  E_LDAP_INVALID_DN_SYNTAX          =   E_FIRST_LDAP_RESULT + 0x22,
  E_LDAP_IS_LEAF                    =   E_FIRST_LDAP_RESULT + 0x23,
  E_LDAP_ALIAS_DEREF_PROBLEM        =   E_FIRST_LDAP_RESULT + 0x24,
  E_LDAP_INAPPROPRIATE_AUTH         =   E_FIRST_LDAP_RESULT + 0x30,
  E_LDAP_INVALID_CREDENTIALS        =   E_FIRST_LDAP_RESULT + 0x31,
  E_LDAP_INSUFFICIENT_RIGHTS        =   E_FIRST_LDAP_RESULT + 0x32,
  E_LDAP_BUSY                       =   E_FIRST_LDAP_RESULT + 0x33,
  E_LDAP_UNAVAILABLE                =   E_FIRST_LDAP_RESULT + 0x34,
  E_LDAP_UNWILLING_TO_PERFORM       =   E_FIRST_LDAP_RESULT + 0x35,
  E_LDAP_LOOP_DETECT                =   E_FIRST_LDAP_RESULT + 0x36,
  E_LDAP_SORT_CONTROL_MISSING       =   E_FIRST_LDAP_RESULT + 0x3C,
  E_LDAP_OFFSET_RANGE_ERROR         =   E_FIRST_LDAP_RESULT + 0x3D,
  E_LDAP_NAMING_VIOLATION           =   E_FIRST_LDAP_RESULT + 0x40,
  E_LDAP_OBJECT_CLASS_VIOLATION     =   E_FIRST_LDAP_RESULT + 0x41,
  E_LDAP_NOT_ALLOWED_ON_NONLEAF     =   E_FIRST_LDAP_RESULT + 0x42,
  E_LDAP_NOT_ALLOWED_ON_RDN         =   E_FIRST_LDAP_RESULT + 0x43,
  E_LDAP_ALREADY_EXISTS             =   E_FIRST_LDAP_RESULT + 0x44,
  E_LDAP_NO_OBJECT_CLASS_MODS       =   E_FIRST_LDAP_RESULT + 0x45,
  E_LDAP_RESULTS_TOO_LARGE          =   E_FIRST_LDAP_RESULT + 0x46,
  E_LDAP_AFFECTS_MULTIPLE_DSAS      =   E_FIRST_LDAP_RESULT + 0x47,
  E_LDAP_VIRTUAL_LIST_VIEW_ERROR    =   E_FIRST_LDAP_RESULT + 0x4c,
  E_LDAP_OTHER                      =   E_FIRST_LDAP_RESULT + 0x50,
  E_LDAP_SERVER_DOWN                =   E_FIRST_LDAP_RESULT + 0x51,
  E_LDAP_LOCAL_ERROR                =   E_FIRST_LDAP_RESULT + 0x52,
  E_LDAP_ENCODING_ERROR             =   E_FIRST_LDAP_RESULT + 0x53,
  E_LDAP_DECODING_ERROR             =   E_FIRST_LDAP_RESULT + 0x54,
  E_LDAP_TIMEOUT                    =   E_FIRST_LDAP_RESULT + 0x55,
  E_LDAP_AUTH_UNKNOWN               =   E_FIRST_LDAP_RESULT + 0x56,
  E_LDAP_FILTER_ERROR               =   E_FIRST_LDAP_RESULT + 0x57,
  E_LDAP_USER_CANCELLED             =   E_FIRST_LDAP_RESULT + 0x58,
  E_LDAP_PARAM_ERROR                =   E_FIRST_LDAP_RESULT + 0x59,
  E_LDAP_NO_MEMORY                  =   E_FIRST_LDAP_RESULT + 0x5a,
  E_LDAP_CONNECT_ERROR              =   E_FIRST_LDAP_RESULT + 0x5b,
  E_LDAP_NOT_SUPPORTED              =   E_FIRST_LDAP_RESULT + 0x5c,
  E_LDAP_NO_RESULTS_RETURNED        =   E_FIRST_LDAP_RESULT + 0x5e,
  E_LDAP_CONTROL_NOT_FOUND          =   E_FIRST_LDAP_RESULT + 0x5d,
  E_LDAP_MORE_RESULTS_TO_RETURN     =   E_FIRST_LDAP_RESULT + 0x5f,

  /* The following errors are defined by Windows NT (i.e. DDK NTSTATUS values).
     Refer to comments in WindowsErrorCodes.h for details. */
  E_FIRST_WINDOWS_NTSTATUS = 10000,

#ifndef SRC_BUILD
  #define NT_CODE(code) E_NTSTATUS_##code,
  #include "WindowsErrorCodes.h"
#endif

} Errno;


/* Function that maps Shark Errno values to AIX errno values.  This routine
   is obsolete and will soon be removed.  Do not use it for new code.  Use
   GPFSToSystemErrno instead. */
extern IntRC SharkToAIXErrno(int err);


/* Map local system error number to a canonical GPFS Errno.  This may be
   used when a local error value comes in from some source other than "errno".
   Use cxiErrno() otherwise. */
extern Errno GPFSErrnoFull(IntRC rc);
static inline Errno GPFSErrno(IntRC rc)
{
  return rc == 0 ? E_OK : GPFSErrnoFull(rc);
}


/* Map an LDAP Results code into a GPFS Errno. */
static inline Errno GPFSLdapErrno(IntRC rc)
{
  return rc == 0 ? E_OK : (Errno)(E_FIRST_LDAP_RESULT + rc);
}


/* Fold special GPFS Errno values into the local error number range */
extern Errno FoldGPFSErrno(Errno err);


#endif /* STANDALONE_UTILITY */


/* Map GPFS Errno value to local system errno value.  Note that this
   mapping loses information because several different GPFS error values
   will map to the same errno.  However, mapping a local system error value
   (numbers less than 200) to a GPFS value and back again will leave the
   number unchanged. */
extern IntRC GPFSToSystemErrnoFull(Errno err);
static inline IntRC GPFSToSystemErrno(Errno err)
{
  return err == E_OK ? 0 : GPFSToSystemErrnoFull(err);
}


/* Map GPFS Errno value to 8 bits values which can be handled by ksh */
extern Errno GPFSToMMErrno(Errno err);


#ifdef __cplusplus

/* StickyErrno class

   StickyErrno is an Errno wrapper class that latches the first error
   assigned to it (the first value != E_OK). Example:
     Errno foo()
     {
       StickyErrno err;
       err = jump();
       err = sit();
       err = rollOver();
       return err;
     }

   This class is only reliable for assignments and loses its stickiness when
   the Errno is returned as a parameters. Also take note of the clear()
   member function:
     Errno foo()
     {
       StickyErrno err;
       err = jump();
       speak(&err);   // Will overwrite a jump error
       err = E_OK;    // this is always a NOOP: use err.clear();
       return err;
     }

   This class makes code easier to read and makes it simpler to retain
   an error in a series of operations. (One might contend that this
   class obfuscates the assignment operator, or promotes lazy handling of
   error conditions. However, this has not been my experience.)
*/
struct StickyErrno
{
  Errno value;

  StickyErrno(Errno err = E_OK)
  {
    value = err;
  }

  Errno clear()
  {
    Errno old = value;
    value = E_OK;
    return old;
  }

  operator Errno&()
  {
    return value;
  }

  StickyErrno& operator =(Errno rhs)
  {
    if (value == E_OK)
      value = rhs;
    return *this;
  }
};
#endif /* __cplusplus */


#if !defined(_KERNEL) && !defined(STANDALONE_UTILITY)

/* Fetch the error return value from a system call and map it to a
   canonical GPFS error number. */
static inline Errno cxiErrno() { return GPFSErrno(errno); }


/* Set the system errno from a GPFS Errno value, mapping the canonical
   value back to a local system value.  Note that this mapping loses
   information because several different GPFS error values will map to the
   same errno.

   Use of this routine should be avoided.  Do not use it to pass back a
   value to another GPFS routine.  Instead, pass back an Errno in the
   return value or a routine argument. */
static inline void cxiSetErrno(Errno e) { errno = GPFSToSystemErrno(e); }


/* This routine is for the rare cases where an errno value from a system
   call is fetched and then immediately passed to an external routine and
   not used within GPFS.  This is equivalent to GPFSToSystemErrno(cxiErrno()),
   but is slightly simpler and more efficient.  */
static inline int cxiLocalErrno() { return errno; }

#endif


/* Change the definition of errno so that it will cause a compiler error if
   it is used.  GPFS code should use cxiErrno() rather than errno.
   Stand-alone utilities can define STANDALONE_UTILITY to avoid this, but
   then they are not allowed to use any GPFS error numbers.

   On AIX, the math.h file references errno, so skip this on AIX systems.
   On recent Linux versions (e.g., RHEL 9, Ubuntu 20.4) errno is
   referenced in /usr/include/c++/9/bits/fstream.tcc.
*/
#if 0
#if !defined(STANDALONE_UTILITY) && !defined(_KERNEL) && !defined(GPFS_AIX) && !defined(PRECOMPILED_HEADERS)
#undef errno
#define errno invalid_use_of_errno
#endif
#endif

/* Some definitions for legitimate uses of local system error numbers.
   For names that are not needed as often, use GPFSToSystemErrno(E_xxxx). */
#ifdef GPFS_AIX
#define LCL_ESTALE 52
#define LCL_ENOSYS 109
#endif
#ifdef GPFS_LINUX
#define LCL_ESTALE 116
#define LCL_ENOSYS 38
#endif
#ifdef GPFS_WINDOWS
#define ESTALE EBADF /* For the benefit of STANDALONE_UTILITIES */
#define LCL_ESTALE ESTALE
#define LCL_ENOSYS 40
#endif
#ifdef GPFS_SOLARIS
#define LCL_ESTALE 151
#define LCL_ENOSYS 89
#endif

#if !defined(STANDALONE_UTILITY) && !defined(DONT_UNDEF_ERROR_NUMBERS) && !defined(PRECOMPILED_HEADERS)
/* Do not let GPFS code use local system error number definitions.  The
   first 34 definitions match on all systems, so allow them for now. */
#undef ENOMSG
#undef EIDRM
#undef ECHRNG
#undef EL2NSYNC
#undef EL3HLT
#undef EL3RST
#undef ELNRNG
#undef EUNATCH
#undef ENOCSI
#undef EL2HLT
#undef EDEADLK
#undef ENOTREADY
#undef EWRPROTECT
#undef EFORMAT
#undef ENOLCK
#undef ENOCONNECT
#undef ESTALE
#undef EDIST
#undef EWOULDBLOCK
#undef EINPROGRESS
#undef EALREADY
#undef ENOTSOCK
#undef EDESTADDRREQ
#undef EDESTADDREQ
#undef EMSGSIZE
#undef EPROTOTYPE
#undef ENOPROTOOPT
#undef EPROTONOSUPPORT
#undef ESOCKTNOSUPPORT
#undef EOPNOTSUPP
#undef EPFNOSUPPORT
#undef EAFNOSUPPORT
#undef EADDRINUSE
#undef EADDRNOTAVAIL
#undef ENETDOWN
#undef ENETUNREACH
#undef ENETRESET
#undef ECONNABORTED
#undef ECONNRESET
#undef ENOBUFS
#undef EISCONN
#undef ENOTCONN
#undef ESHUTDOWN
#undef ETIMEDOUT
#undef ECONNREFUSED
#undef EHOSTDOWN
#undef EHOSTUNREACH
#undef ERESTART
#undef ECLONEME
#undef EPROCLIM
#undef EUSERS
#undef ELOOP
#undef ENAMETOOLONG
#undef ENOTEMPTY
#undef EDQUOT
#undef ECORRUPT
#undef EREMOTE
#undef ENOSYS
#undef EMEDIA
#undef ESOFT
#undef ENOATTR
#undef ESAD
#undef ENOTRUST
#undef ETOOMANYREFS
#undef EILSEQ
#undef ECANCELED
#undef ENOSR
#undef ETIME
#undef EBADMSG
#undef EPROTO
#undef ENODATA
#undef ENOSTR
#undef ENOTSUP
#undef EMULTIHOP
#undef ENOLINK
#undef EOVERFLOW
#undef EBADE
#undef EBADR
#undef EXFULL
#undef ENOANO
#undef EBADRQC
#undef EBADSLT
#undef EBFONT
#undef ENONET
#undef ENOPKG
#undef EADV
#undef ESRMNT
#undef ECOMM
#undef EDOTDOT
#undef ENOTUNIQ
#undef EBADFD
#undef EREMCHG
#undef ELIBACC
#undef ELIBBAD
#undef ELIBSCN
#undef ELIBMAX
#undef ELIBEXEC
#undef ESTRPIPE
#undef EUCLEAN
#undef ENOTNAM
#undef ENAVAIL
#undef EISNAM
#undef EREMOTEIO
#undef ENOMEDIUM
#undef EMEDIUMTYPE
#undef ENOKEY
#undef EKEYEXPIRED
#undef EKEYREVOKED
#undef EKEYREJECTED
#undef EZERO
#undef EBADENCRYPTDATA
#undef ENOSETUID
#undef EFTYPE
#undef EWINPATH
#undef E2SMALL
#undef EHOSTNOTFOUND
#undef ETRYAGAIN
#undef ENORECOVERY

#endif /* STANDALONE_UTILITY */


/* Linux NLS Values for YESSTR & NOSTR */
typedef enum
{
  LINUX_YESSTR = 327682,
  LINUX_NOSTR =  327683
}LinuxNlsVal;

/* AIX NLS Values for YESSTR & NOSTR */
typedef enum
{
   AIX_YESSTR = 46,
   AIX_NOSTR  = 47
}AIXNlsVal;

/* Windows NLS Values for YESSTR & NOSTR */
typedef enum
{
   WINDOWS_YESSTR = 18,
   WINDOWS_NOSTR  = 19
}WindowsNlsVal;

/* Solaris NLS Values for YESSTR & NOSTR */
typedef enum
{
   SOLARIS_YESSTR = 41,
   SOLARIS_NOSTR  = 42
}SNlsVal;

extern int SystemToGPFSNlsVal(int msgNum);
extern int GPFSToSystemNlsVal(int msgNum);

#if defined(GPFS_SOLARIS)
#ifdef __cplusplus
}
#endif
#endif  /* GPFS_SOLARIS */

#endif /* _h_ShErrno */
