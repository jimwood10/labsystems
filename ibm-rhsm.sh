#!/bin/bash
Version=20250530.1
############################################################################
#
#               ------------------------------------------
#               THIS SCRIPT PROVIDED AS IS WITHOUT SUPPORT
#               ------------------------------------------
#
# Questions/feedback: https://ltc.linux.ibm.com/support/ltctools.php
#
# Please see https://ltc.linux.ibm.com/blurb/ for announcements regarding
# new versions of this script.
#
# Description: Wrapper script for subscription-manager to register RHEL 6,
#              7, 8, 9, and 10 systems with the internal Red Hat Satellite
#              using FTP3 credentials.
#
# The following environment variables can be used:
#
#   FTP3USER=user@cc.ibm.com        FTP3 Account
#   FTP3PASS=mypasswd               FTP3 Password
#   FTP3FORCE=y                     FTP3 force registration
#
# You must be root to perform registration activities with this script. The user
# id and password will be prompted for if the environment variables are not set.
#
# example uses might be:
#
#  1.  ./ibm-rhsm.sh --register
#  2.  FTP3USER=user@cc.ibm.com ./ibm-rhsm.sh --register
#  3.  ./ibm-rhsm.sh --list-systems
#  4.  ./ibm-rhsm.sh --delete-systems
#
# The first example is a good way to test this script.
# The second example shows how to set the FTP3USER environment variable on
# the command line.
# The third example gets the list of registered systems you own.
# The fourth example removes all systems you own from your subscription.
#
# NOTE: Some parts of this script were extracted
#       from the good old ibm-yum.sh script.
#
############################################################################

## if xtrace flag activated the activate also verbose for debugging
[[ $(set -o |grep xtrace|cut -d' ' -f10|cut -f2) == "on" ]] && set -o verbose

if [ -z "$IBM_RHSM_REG_LOG" ] ; then
    IBM_RHSM_REG_LOG="ibm-rhsm.log"
fi
## default host
[[ -z "$FTP3HOST" ]] && FTP3HOST="ftp3.linux.ibm.com"
API_URL=
ORGANIZATION="Default_Organization"
SUPPORT_URL=https://ltc.linux.ibm.com/support/ltctools.php
REPORT="This could be reported at: $SUPPORT_URL
Please do not forget to add the $IBM_RHSM_REG_LOG file to the request."

CERTHUB="https://certhub.digitalcerts.identity-services.intranet.ibm.com"
IBMROOT_URL="$CERTHUB/root.cer"
IBMINT_URL="$CERTHUB/intermediate.cer"

## supported values
SUPPORTED_RELEASES=(6 7 8 9 10)
SUPPORTED_VERSIONS=(server workstation client release)
SUPPORTED_ARCHS=(x86_64 ppc64le ppc64 s390x aarch64)

## Script progress
##   warmup:  beginning of the process, mostly checking small things ; it has no meaning to clean up on exit
##   parsing:  processing user input
##   initializing:  preparing to perform the specified action
##   reporting:  running an action other than registering
##   registering: running the real stuff ; clean up and log data on exit
##   ok: registration completed successfully
PROGRESS="warmup"

## CLI action
ACTION=

## Functions

## log string and/or command to logfile
##   logthis -E "send string to both console and log"
##   logthis -n "send string to console without newline, and to log (with newline)"
##   logthis -s "send string to log only"
## The command and its result will be logged:
##   logthis cmd arg1 arg2 ...
## warning: | and " must be escaped
function logthis() {
  local arg=$1
  if [[ $arg == "-E" || $arg == "-n" ]]; then
    shift
    echo $arg "$@"
    arg="-s"
  fi
  if [[ $arg == "-s" ]]; then
    if [[ "$1" == "-s" ]]; then
      shift
    fi
    echo "$@" >> $IBM_RHSM_REG_LOG
    return
  fi
  [[ $# -le 0 ]] && return
  echo "--" "$@" >> $IBM_RHSM_REG_LOG
  eval "$@" &>> $IBM_RHSM_REG_LOG
}

## initialize the log file
if [[ -z "${FTP3LOGAPPEND+x}" ]]; then
  rm -f $IBM_RHSM_REG_LOG
fi
logthis -s "-- $IBM_RHSM_REG_LOG --------------------------------------------------------"
logthis -s "On $(date)"
logthis -s "Script version: $Version"
logthis -s "Program $BASH_SOURCE invoked with the following arguments:"
for arg in "$@" ; do
  logthis -s "  $arg"
done
dollar_at=$@

if [ "${FTP3DEBUG+set}" == "set" ] ; then
  opt="-E"
  logthis $opt "FTP3DEBUG is defined, emitting additional information"
else
  opt="-s"
fi
logthis -s "System: $(uname -a)"
logthis lscpu
logthis cat /etc/redhat-release

## $1: string to print
## $2: color code as defined 0 = green; 1 = red; 2 = yellow
function formatted_echo() {
  value=0
  case $2 in
    0) value=32 ;;
    1) value=31 ;;
    2) value=33 ;;
  esac
  echo -e "\r\t\t\t\t\t\t\t\e[${value}m$1\e[0m"
}

exitmsg="Aborting due to command failure"

# run_command
#
# Run a shell command that captures stdout, stderr, and the result code to
# variables that are subsequently accessible to the caller.  At the completion
# of this function, the following variables will be available to the caller:
#
#   stdout - All standard output produced by the command
#   stderr - All error output produced by the command
#   stdexit - The exit code returned by the command
#
# If the "X" variable is set to non-zero and the invoked command exits with a
# non-zero value, the script will terminate.
X=0
function run_command() {
  local save_ifs="$IFS"
  logthis -s "-- $@"
  {
    IFS=$'\n' read -r -d '' stderr;
    IFS=$'\n' read -r -d '' stdout;
    IFS=$'\n' read -r -d '' stdexit;
  } < <((printf '\0%s\0%d\0' "$(IFS=" " ; eval $@)" "${?}" 1>&2) 2>&1)
  stdexit=${stdexit:-0};
  IFS="$save_ifs"
  if [[ $stdexit -ne 0 && $X -ne 0 ]]; then
    echo $stderr >> $IBM_RHSM_REG_LOG
    echo $exitmsg
    exit $stdexit
  fi
}

have_curl=0
function require_curl {
  if [ $have_curl -eq 1 ]; then
    return
  fi
  logthis $opt "Verifying that curl is installed:"
  # First check to make sure the curl command is installed.
  run_command type -p curl
  if [[ $stdexit -ne 0 ]]; then
    logthis -E "The 'curl' command is not installed; unable to proceed."
    exit 1
  fi
  have_curl=1
}

have_ibm_root_ca=0
function require_ibm_root_ca {
  if [ $have_ibm_root_ca -eq 1 ]; then
    return
  fi
  # If the signing certificate for the IBM Internal Root Certificate Authority
  # is not installed, secure connections to the server will not succeed.  Query
  # the local trust store to see if the CA certificate is installed.
  logthis $opt "Checking to see if IBM Internal Root CA certificate is installed."
  run_command 'trust list | grep -q "IBM Internal Root CA" 2> /dev/null'
  if [[ $stdexit -ne 0 ]]; then
    logthis -E "The security certificate presented by $FTP3HOST"
    logthis -E "is signed and issued by the IBM Internal Certificate Authority (CA), but"
    logthis -E "the IBM Internal Root certificate provided by the CA is not installed and"
    logthis -E "trusted on this machine, thus secure connections cannot be established.  Do"
    logthis -n "you wish to download, trust, and install this certificate? (y/n):"
    local PROCEED
    read PROCEED
    if [[ "${PROCEED,,}" != "y" ]]; then
      logthis -E "Aborting..."
      echo
      exit 1
    fi
    logthis -E "Proceed with installing certificate."
    require_curl
    pushd /tmp > /dev/null
    X=1 run_command curl --insecure -o root.cer $IBMROOT_URL
    X=1 run_command trust anchor /tmp/root.cer
    run_command rm -f /tmp/root.cer
    # Not strictly needed for FTP3 operations, but if the intermediate certificate
    # is not installed, go ahead and get that as well.
    logthis $opt "Checking to see if IBM Internal Intermediate CA certificate is installed."
    run_command 'trust list | grep -q "IBM INTERNAL INTERMEDIATE CA" 2> /dev/null'
    if [[ $stdexit -ne 0 ]]; then
      X=1 run_command curl --insecure -o intermediate.cer $IBMINT_URL
      X=1 run_command trust anchor /tmp/intermediate.cer
      run_command rm -f /tmp/intermediate.cer
    fi
    popd > /dev/null
  fi
  have_ibm_root_ca=1
}

unset user_enc
have_user=0
function require_user {
  if [ $have_user -eq 1 ]; then
    return
  fi
  logthis $opt "Requiring FTP3 user"
  if [[ -z "$FTP3USER" ]]; then
    logthis $opt "Prompting for FTP3USER."
    echo -n "User ID: "
    read FTP3USER

    if [ -z "$FTP3USER" ] ; then
      logthis -E ""
      logthis -E "Missing userid. Either set the environment variable"
      logthis -E "FTP3USER to your user id or enter a user id when prompted."
      logthis -E "Goodbye."
      logthis -E ""
      exit 1
    fi
  fi
  logthis $opt "FTP3USER=$FTP3USER"

  # Encode the the username for use in URLs
  user_enc=$(echo $FTP3USER | sed s/@/%40/g)
  have_user=1
}

unset pass_enc
have_password=0
function require_password {
  if [ $have_password -eq 1 ]; then
    return
  fi
  logthis $opt "Requiring password authentication"
  if [[ -z "$FTP3PASS" && -n "$FTP3PWFILE" ]]; then
    if [ -e "$FTP3PWFILE" ]; then
      read -r FTP3PASS < $FTP3PWFILE
    else
      logthis -E "Password file $FTP3PWFILE does not exist"
    fi
  fi

  if [ -z "$FTP3PASS" ]; then
    ## prompt for password
    require_user
    logthis -s "Prompting for FTP3PASS."
    echo -n "Password for $FTP3USER: "
    stty -echo
    read -r FTP3PASS
    stty echo
    echo ""
    echo ""

    if [ -z "$FTP3PASS" ] ; then
      logthis -E "Missing password. Either set the environment variable"
      logthis -E "FTP3PASS to your user password or enter a password when"
      logthis -E "prompted. Goodbye."
      logthis -E ""
      exit 1
    fi
  fi

  if [ -z "$pass_enc" ]; then
    # Encode user password for use in URLs
    pass_enc=$(echo -n $FTP3PASS | od -tx1 -An | tr -d '\n' | sed 's/ /%/g')
  fi
  have_password=1
}

have_authentication=0
function require_authentication {
  if [ $have_authentication -eq 1 ]; then
    return
  fi
  logthis $opt "Requiring authentication"
  require_user
  require_password
  have_authentication=1
}

# next_xml_tag()
#
# Simple parse of the current location in the XML stream to recognize the next
# XML tag and isolate any associated data.
#
# Implementing a full-blown XML parser using bash is pretty much out of the
# question, and the data returned from the FTP3 XML/RPC API interface is very
# rudimentary so we don't even bother trying.  Just do simple pattern matching
# for <tag></tag> strings and any text content enclosed between them.
function next_xml_tag () {
  local result=0
  local IFS=\>
  read -d \< TAG CONTENT
  local ret=$?
  if [[ $ret -ne 0 && -z "$TAG" ]]; then
    # Reading the last line will set ret=1 even if there is data
    result=1
  fi
  # The FTP3 XML/RPC API doesn't generate any XML that contains attributes,
  # but we'll go ahead and isolate them here.
  TAG_NAME=${TAG%% *}
  ATTRIBUTES=${TAG#* }
  # Strip embedded newlines (see "QUOTING" in the bash man page for $'string')
  #CONTENT=${CONTENT//[$'\n']}
  # remove trailing whitespace characters
  #CONTENT="${CONTENT%"${CONTENT##*[![:space:]]}"}"
  # return 0 if data was parsed, 1 if end of input
  return $result
}

# Global variables to hold the data parsed from the XML stream
declare -i BOOLEAN
declare    STRING
declare -a STRINGS

# Global variable denoting the type of data returned (last one wins)
declare TYPE

# Global variables to keep track of tag nesting
declare -i in_array
declare -i in_boolean
declare -i in_string

# process_tag()
#
# Interpret the most recently parsed XML tag (assumed to be in the global
# TAG_NAME variable) and keep track of whether it represents something of
# interest.  If it's an <array> tag, increment the current "in_array" nesting
# level; decrement the level if it's an </array> tag.  If it's a <boolean> tag,
# store any associated data in the global BOOLEAN variable.  If it's a <string>
# tag and the "in_array" nesting level is non-zero, append any associated data
# to the STRINGS array, otherwise store the data in the scalar STRING variable.
function process_tag () {
  # Test the ability to recognize attributes, even though we don't use them
  #if [[ "$TAG_NAME" == "?xml" ]]; then
  #  eval local $ATTRIBUTES
  #  echo "xml version=$version encoding=$encoding"
  #fi
  if [[ -n "$TAG_NAME" ]]; then
    case "$TAG_NAME" in
      array)
        ((in_array++))
        ;;
      boolean)
        ((in_boolean++))
        if [[ "$CONTENT" ]]; then
          BOOLEAN=$CONTENT
          TYPE="boolean"
        fi
        ;;
      string)
        ((in_string++))
       #if [[ "$CONTENT" ]]; then
          if [[ $in_array -ne 0 ]]; then
            STRINGS+=("$CONTENT")
            TYPE="strings"
          else
            STRING="$CONTENT"
            TYPE="string"
          fi
       #fi
        ;;
      /array)   ((in_array--))   ;;
      /boolean) ((in_boolean--)) ;;
      /string)  ((in_string--))  ;;
    esac
  fi
}

function process_xml () {
  in_array=0
  in_boolean=0
  in_string=0
  STRING=""
  STRINGS=()
  TYPE=""
  while next_xml_tag ; do
    process_tag
  done
}

# do_action()
#
# Submit an action against the FTP3 XML/RPC API interface (using curl) and
# set the return code ($?) to 0 for success and non-zero for failure.  If
# the return code is 0, the TYPE global variable will be set to one of
# "boolean", "string", or "strings" to indicate the type of data returned,
# and the results will be stored accordingly in one of the following global
# variables:
#
#   BOOLEAN - an integer value of either 0 or 1
#   STRING  - a single string value
#   STRINGS - an array of string values
#
# If the return code is non-zero, an error will have been logged (no need
# for the caller to do this) and the STRING variable will contain the failure
# string that was logged.
#
# The following invocations are supported:
#
#   do_action CREATE_KEY       type satellite
#   do_action DELETE_SYSTEMS   type satellite system
#   do_action LIST_SATELLITES
#   do_action LIST_SYSTEMS     type satellite
#   do_action SCRIPT_LOCATION  distro
#   do_action SCRIPT_VERSION   distro
#   do_action SHOW_KEY         type satellite
#   do_action SHOW_SATELLITE   type satellite
# 
function do_action() {
  require_curl
  require_ibm_root_ca
  local action="$1"
  # advance past the action
  shift

  local activity
  local command

  local rtype="string"
  local -a params
  local nargs=0

  local need_auth=0
  local need_ver=1
  local need_rhsm=0
  case "$action" in
    CREATE_KEY)
      activity="creating the user's activation key"
      command="user.create_activation_key2"
      nargs=2
      need_auth=1
      ;;
    DELETE_SYSTEMS)
      activity="attempting to delete registered systems"
      command="user.delete_registered_systems2"
      nargs=3
      need_auth=1
      ;;
    LIST_SATELLITES)
      activity="querying the list of current satellites"
      command="user.list_satellites"
      ;;
    LIST_SYSTEMS)
      activity="querying the list of registered systems"
      command="user.list_registered_systems2"
      nargs=2
      need_auth=1
      ;;
    SCRIPT_LOCATION)
      activity="querying the current script location"
      command="user.script_location"
      nargs=1
      need_ver=0
      need_rhsm=1
      ;;
    SCRIPT_VERSION)
      activity="querying the current script version"
      command="user.script_version"
      nargs=1
      need_ver=0
      need_rhsm=1
      ;;
    SHOW_KEY)
      activity="querying the user's activation key"
      command="user.find_activation_key"
      nargs=2
      need_auth=1
      ;;
    SHOW_SATELLITE)
      activity="querying the current satellite"
      command="user.show_satellite"
      nargs=2
      ;;
    *)
      return 1
  esac

  if [ $need_ver -eq 1 ]; then
    params+=($Version)
  fi

  if [ $need_auth -eq 1 ]; then
    require_authentication
    params+=($user_enc)
    params+=($pass_enc)
  fi

  if [[ $nargs -gt 0 ]]; then
    for (( i=0; i<${nargs}; i++ )); do
      params+=("$1")
      shift
    done
  fi

  if [ $need_rhsm -eq 1 ]; then
    params+=("rhsm")
  fi

  params_xml=""
  nparams=${#params[@]}
  for (( i=0; i<${nparams}; i++ )); do
    params_xml+=" <param><value>${params[$i]}</value></param>"
  done

  run_command curl --silent --show-error $API_URL -H "\"Content-Type: text/xml\"" -d "\"<?xml version='1.0' encoding='UTF-8'?><methodCall><methodName>${command}</methodName> <params> $params_xml </params> </methodCall>\""

  if [[ $stdexit -ne 0 ]]; then
    logthis -E "stderr: $stderr"
    logthis -E "Error $stdexit has occurred while $activity."
    return 1
  fi
  process_xml <<< "$stdout"
  if [[ -z "$TYPE" ]]; then
    logthis -E "$action error: Did not receive expected data"
    return 1
  elif [[ "$TYPE" != "$rtype" ]]; then
    logthis -E "$action error: $STRING"
    return 1
  fi
  return 0
}

## $1: action, one of:
##     CREATE_KEY
##     DELETE_SYSTEMS
##     SHOW_KEY
##     SHOW_SATELLITE
##     LIST_SATELLITES
##     LIST_SYSTEMS
## $2: exit code from do_action
## $3: do_action result
check_result() {
  action=$1
  action_rc=$2
  action_result=$3
  case "$action" in
    CREATE_KEY | DELETE_SYSTEMS | SHOW_KEY | SHOW_SATELLITE | LIST_SATELLITES | LIST_SYSTEMS | SCRIPT_VERSION)
      ;;
    *)
      return
      ;;
  esac
  if [[ $action_rc -ne 0 ]]; then
    formatted_echo "FAIL" 1
    logthis -E "Error $action_rc performing $action action
$action_result
Aborting...
"
    exit $action_rc
  elif [[ -z "$action_result" ]]; then
    formatted_echo "FAIL" 1
    echo
    case "$action" in
      SCRIPT_VERSION)
        logthis -E "Unable to determine script version"
        ;;
      CREATE_KEY | SHOW_KEY)
        logthis -E "An error has occurred: No activation key.
There was a problem while querying or creating your activation key.
Please make sure you are connected to the IBM network and using a valid FTP3 account.
Aborting."
        ;;
      DELETE_SYSTEMS)
        logthis -E "No registered systems found."
        ;;
      SHOW_SATELLITE)
        logthis -E "Unable to retrieve name of LTC Redhat Satellite."
        ;;
      LIST_SATELLITES)
        logthis -E "No active satellites found."
        ;;
      LIST_SYSTEMS)
        logthis -E "No registered systems found."
        ;;
    esac
    echo
    exit 1
  elif [[ "$action_result" == "Account not found" || "$action_result" == "Wrong username or password" ]]; then
    formatted_echo "FAIL" 1
    logthis -E "An error has occurred: $action_result
Please make sure you're using the correct FTP3 username and password.
Aborting."
    exit 1
  elif [[ "$action_result" == "The account $FTP3USER does not have access to Red Hat content" ]]; then
    formatted_echo "FAIL" 1
    logthis -E "An error has occurred: $action_result
You may request access on the "My Account" page: https://$FTP3HOST/myaccount/access.php.
Aborting."
    exit 1
  elif [[ "$3" == "No activation key was found" ]]; then # This could only happen if $1 is DELETE_SYSTEMS or LIST_SYSTEMS
    formatted_echo "FAIL" 1
    logthis -E "An error has occurred: No activation key.
Please make sure you are connected to the IBM network and using a valid FTP3 account.
Aborting."
    exit 1
  elif [[ "$action_result" == "Two (or more) activation keys were found" ]]; then
    formatted_echo "FAIL" 1
    logthis -E "An error has occurred: $action_result
You may open a ticket here: ${SUPPORT_URL}?Tool=FTP3
Please add this message to the new ticket.
Aborting."
    exit 1
  elif [[ "$action" == "DELETE_SYSTEMS" && "$action_result" == "Unable to find this system: "* ]]; then
    formatted_echo "FAIL" 1
    logthis -E "An error has occurred: $action_result
Please make sure you own the system you're trying to delete and that you're using the correct hostname.
To check which registered systems you own: $0 --list-systems"
    exit 1
  elif [[ "$action" == "DELETE_SYSTEMS" && "$action_result" == *"KO:"* ]]; then
    if [[ "$action_result" == *"OK:"* ]]; then
      formatted_echo "WARN" 2
    else
      formatted_echo "FAIL" 1
    fi
  else
    formatted_echo "OK" 0
  fi
}

## print usage
function usage() {
  result=1
  [[ $1 != "" ]] && result=$1
  cat <<EOF
IBM Linux Technology Center Redhat Satellite registration script.
Use of this script requires a $FTP3HOST user account.

Usage:
  $0 [OPTIONS] ACTION

OPTIONS:
  --force
      Don't ask for confirmation before re-registering a system that is already
      registered.

  --ftp3 <hostname>
      Use a specific host name to refer to the FTP3 server.  This should only be
      used for testing, debugging, or as directed by LTC support personnel.
      The FTP3HOST environment variable will be queried if this value is not
      specified, otherwise the default value of ftp3.linux.ibm.com will be used.

  --satellite <hostname>
      Use a specific Redhat Satellite host for all subsequent operations.  This
      should only be used for testing, debugging, or as directed by LTC support
      personnel.

  --type <satellite-type>
      Only consider Redhat Satellites of a specific type.  This should only be
      used for testing, debugging, or as directed by LTC support personnel.  The
      default type is "production".  A pseudo value of "all" is allowed.

  --user <ftp3-ID>
      The name of your FTP3 user ID.  The FTP3USER environment variable will be
      queried if this value is not specified, otherwise the program will prompt
      for this value.

  --password <ftp3-password>
      The password of your FTP3 user ID.  The FTP3PASS environment variable will
      be queried if this value is not specified, otherwise the program will
      prompt for this value.

ACTIONs:
  --delete-systems [<system>]
      Delete system registrations from the Red Hat Satellite without involving
      the subscription-manager utility.  Deletes all the registered systems you
      own or just the given one.  Must be specified as the last parameter if an
      argument is given.

  --list-satellites
      Print the names of all defined LTC Rehat Satellites.

  --list-systems
      Print the registered systems assigned to your activation key.

  --register
      Register this system with the LTC Redhat Satellite.

  --show-key
      Display any existing activation key associated with your FTP3 ID.

  --show-satellite
      Query and display the name of the production LTC Redhat Satellite.

  --unregister
      Unregister this system from the LTC Redhat Satellite.

  --version
      Display program version number and exit.

  Actions are mutually exclusive.  There is no default action.

EOF
  exit $result
}

KATELLO_CERT_RPM="uninitialized"
## this is called on exit
function clean_up() {
  result=$?
  [[ "$PROGRESS" == "parsing" \
  || "$PROGRESS" == "initializing" \
  || "$PROGRESS" == "reporting" ]] && exit $result
  [[ "$PROGRESS" == "ok" ]] && exit 0

  if [[ "$PROGRESS" == "registration" ]]; then
    logthis -s "-- Cleaning on exit ----------------------------------------------------"
    if rpm --quiet -q $KATELLO_CERT_RPM; then
      echo "Cleaning up..."
      logthis -s "$KATELLO_CERT_RPM is installed, removing."
      run_command rpm --quiet -e $KATELLO_CERT_RPM
    fi
    logthis subscription-manager facts
    logthis tail -30 /var/log/rhsm/rhsm.log
  fi
  exit 1
}

## clean up proper if something goes bad
trap clean_up EXIT HUP INT QUIT TERM;

function get_argument() {
  variable=$1
  parameter=$2
  argument=$3
  if [[ $# -eq 3 ]]; then
    eval "$variable=$argument"
  else
    echo "The $parameter option requires an argument."
    echo "Run \"$0 --help\" for assistance."
    exit 2
  fi
  return 0
}

## check CLI parameters
PROGRESS="parsing"
action_count=0
SATELLITE=""
STYPE="production"
SYSTEM_TO_DELETE=
while [[ $# -gt 0 ]]; do
  if [[ "$1" == -* ]]; then
    parameter="$1"
    case "$parameter" in
      --force)
        FTP3FORCE=y
        ;;
      --ftp3)
        shift
        get_argument FTP3HOST $parameter $1
        ;;
      --user)
        shift
        get_argument FTP3USER $parameter $1
        ;;
      --password)
        shift
        get_argument FTP3PASS $parameter $1
        ;;
      --list-satellites)
        ACTION="LIST_SATELLITES"
        ((action_count++))
        ;;
      --list-systems)
        ACTION="LIST_SYSTEMS"
        ((action_count++))
        ;;
      --satellite)
        shift
        get_argument SATELLITE $parameter $1
        ;;
      --type)
        shift
        # Because the --type argument is allowed to be the empty string,
        # we must quote all instances of it.
        if [[ $# -eq 0 ]]; then
          get_argument STYPE $parameter $1
        else
          get_argument STYPE $parameter "$1"
        fi
        ;;
      --register)
        ACTION="REGISTER"
        ((action_count++))
        ;;
      --unregister)
        ACTION="UNREGISTER"
        ((action_count++))
        ;;
      --show-key)
        ACTION="SHOW_KEY"
        ((action_count++))
        ;;
      --show-satellite)
        ACTION="SHOW_SATELLITE"
        ((action_count++))
        ;;
      --delete-systems)
        ACTION="DELETE_SYSTEMS"
        ((action_count++))
        shift
        # Optional argument
        if [[ -n "$1" ]]; then
          SYSTEM_TO_DELETE="$1"
        fi
        ;;
      --help)
        usage 0
        ;;
      --version)
        echo "version $Version"
        exit 0
        ;;
      *)
        echo "Invalid parameter: $1"
        echo "Run \"$0 --help\" for assistance."
        exit 2
        ;;
    esac
    shift
  else
    break
  fi
done
if [ $action_count -eq 0 ]; then
  echo "No action was specified."
  echo "Run \"$0 --help\" for assistance."
  exit 2
fi
if [ $action_count -gt 1 ]; then
  echo "Action parameters are mutually exclusive."
  echo "Run \"$0 --help\" for assistance."
  exit 2
fi
[[ $# -gt 0 ]] && usage

if [[ "$ACTION" == "REGISTER" || "$ACTION" == "DELETE_SYSTEMS" || "$ACTION" == "UNREGISTER" ]]; then
  ## must be root to register
  if [[ $(whoami) != "root" ]] ; then
    echo "You must be root to run registration operations. Goodbye."
    echo
    exit 1
  fi
fi

## The API through which we talk to FTP3
API_URL="https://$FTP3HOST/rpc/index.php"

## Force path with /usr/sbin for subscription-manager need
export PATH=/usr/sbin:$PATH

PROGRESS="initializing"

logthis -s "-- Query latest script version -----------------------------------------"
echo -n "Querying script version..."
do_action SCRIPT_VERSION redhat
RET=$?
logthis $opt "return code: $RET"
SCRIPT_VERSION="$STRING"
logthis -s "SCRIPT_VERSION=$SCRIPT_VERSION"
check_result SCRIPT_VERSION $RET "$SCRIPT_VERSION"
logthis -s "Comparing version information:"
latest=$(printf "%03d%03d%03d" ${SCRIPT_VERSION//./ })
this=$(printf "%03d%03d%03d\n" ${Version//./ })
logthis -s "latest script version:  $latest"
logthis -s "version of this script: $this"
if [ "$latest" \> "$this" ]; then
  if [ -z "$FTP3USER" ]; then
    user="<ftp3-id>"
  else
    user=$FTP3USER
  fi
  REFPATH=${BASH_SOURCE}
  REALPATH=$(realpath $REFPATH)
  logthis -E "Obsolete script version $Version is replaced by version $SCRIPT_VERSION."
  logthis -E "Script referenced as: $REFPATH"
  logthis -E "Local script location: $REALPATH"

  logthis $opt "Querying remote script location:"
  do_action SCRIPT_LOCATION redhat
  if [[ $? -ne 0 ]]; then
    exit 1
  fi
  # Must parse for error since both success and failure are returned as a string
  if [[ "${STRING:0:6}" == "Error:" ]]; then
    logthis -E $STRING
    exit 1
  fi
  SCRIPT_LOCATION=$STRING
  logthis -s "SCRIPT_LOCATION=$SCRIPT_LOCATION"
  SCRIPT=${SCRIPT_LOCATION##*/}

  https_url="https://$FTP3HOST/files${SCRIPT_LOCATION}"

  PROCEED="y"
  if [[ -z "${FTP3FORCE+x}" ]]; then
    logthis -E "Would you like to download and install the new version to the"
    logthis -n "location $REALPATH? (y/n): "
    read PROCEED
  fi
  if [[ "${PROCEED,,}" != "y" ]]; then
    logthis -E "The latest version of the script can be downloaded by running one of:"
    logthis -E "curl --user $user -o $REALPATH $https_url"
    logthis -E "wget --user $user --ask-password -O $REALPATH $https_url"
    logthis -E "Aborting..."
    echo
    exit 1
  fi

  require_authentication
  tmpfile=$(mktemp)
  # The do_action() call above already ran require_curl()
  command="curl"
  command+=" --user $FTP3USER:$FTP3PASS"
  command+=" -o $tmpfile"
  vcommand="$command --silent --show-error --write-out %{http_code} --head"
  script_size=0
  command+=" $https_url"
  vcommand+=" $https_url"
  need_code=200

  logthis -E "Verifying attributes of remote file"
  run_command $vcommand
  if [ "$stdout" != "$need_code" ]; then
    logthis -E "Unexpected result $stdout querying remote file"
    if [ -n "$stdout" ]; then
      logthis -E "stderr: $stderr"
    fi
    rm -f $tmpfile
    exit 1
  else
    # The downloaded headers should show the size of the remote file
    run_command gawk -v IGNORECASE=1 "'/^Content-Length/ { print \$2 }'" $tmpfile
    # Strip whitespace from output
    content_length=${stdout//[$'\t\r\n ']}
    logthis -s "Expected file size: $content_length"
    # Ensure content length is numeric
    if [[ "$content_length" =~ ^-?[0-9]+$ && $content_length -gt 0 ]]; then
      script_size=$content_length
    else
      logthis -E "Unexpected size value $content_length querying remote file"
      rm -f $tmpfile
      exit 1
    fi
  fi

  logthis -E "Downloading checksum file for $SCRIPT"
  X=1 run_command "$command.sum"
  X=1 run_command awk "'{print \$1}'" $tmpfile
  checksum=$stdout
  logthis -s "checksum=$checksum"

  logthis -E "Downloading $SCRIPT"
  X=1 run_command "$command"
  # Verify the validity of the download via simple checksum
  logthis -E "Validating the download"
  X=1 run_command stat "--printf=\"%s\"" $tmpfile
  file_size=$stdout
  logthis -s "Downloaded file size: $file_size"
  if [[ $script_size -ne $file_size ]]; then
    logthis -E "Size of downloaded file ($file_size) does not match the expected size ($script_size)"
    rm -f $tmpfile
    exit 1
  fi
  logthis -s "Downloaded file size is correct"
  logthis -E "Verifying file checksum"
  X=1 run_command "echo $checksum $tmpfile | sha256sum --check --quiet"

  logthis -E "Installing $SCRIPT to $REALPATH"
  X=1 run_command chown --reference=$REALPATH $tmpfile
  X=1 run_command chmod --reference=$REALPATH $tmpfile
  # Synchronize cached writes to persistent storage
  X=1 run_command sync

  # Move the downloaded file to the location of the original script
  X=1 run_command mv $tmpfile $REALPATH

  logthis -E "Reinvoking updated script with original arguments"
  sync
  # If require_authentication prompted for a user or password, export those
  # variables here so the re-invoked script doesn't prompt for them again.
  export FTP3USER
  export FTP3PASS
  # Don't trash the log entries for the download activity.
  export FTP3LOGAPPEND=true
  exec $REFPATH $dollar_at
fi
logthis $opt "Script is up to date."

# See if the version of this script was overridden (for debugging purposes only!):
if [ -v FTP3VER ]; then Version=$FTP3VER ; fi

if [[ "$ACTION" == "LIST_SATELLITES" ]]; then
    echo -n "Searching for defined satellites... "
fi

logthis -s "-- Query defined satellites --------------------------------------------"
do_action LIST_SATELLITES
RET=$?
DEFINED_SATELLITES="$STRING"
logthis $opt "return code: $RET"
logthis $opt "result: $DEFINED_SATELLITES"

S=""
ACTIVE_SATELLITES=""
# Each satellite entry has the form:
#     host(name1=value1:name2=value2:name3=value3)
for defined_satellite in $DEFINED_SATELLITES; do
  # Isolate the host name
  host=${defined_satellite%(*}
  # Remove leading host name and open paren
  attributes=${defined_satellite#*(}
  # Remove the closing paren
  attributes=${attributes%)}
  # Convert colons to spaces
  attributes=${attributes//:/ }
  for attribute in $attributes ; do
    name=${attribute%=*}
    value=${attribute#*=}
    if [[ "$name" == "state" && ("$value" == "active" || "$value" == "obsolete") ]] ; then
      ACTIVE_SATELLITES="${ACTIVE_SATELLITES}${S}${host}"
      S=" "
    fi
  done
done
logthis $opt "Active satellites: $ACTIVE_SATELLITES"

if [[ "$ACTION" == "LIST_SATELLITES" ]]; then
  logthis $opt "-- List satellites -----------------------------------------------------"
  check_result LIST_SATELLITES $RET "$DEFINED_SATELLITES"
  echo
  echo $DEFINED_SATELLITES | tr ' ' '\n' | sed "s/^/    /"
  echo
  for defined_satellite in $DEFINED_SATELLITES; do
    logthis $opt "    $defined_satellite"
  done
  exit 0
fi

# Get the name of the production LTC Redhat Satellite
logthis $opt "-- Query satellite -----------------------------------------------------"
do_action SHOW_SATELLITE "$STYPE" "$SATELLITE"
if [[ "$STRING" == SCRIPT_OBSOLETE* ]]; then
  logthis -E "$STRING"
  exit 1
fi
if [[ "$STRING" == "" ]]; then
  logthis -E "No eligible satellites found."
  exit 1
fi
LTC_SATELLITE=$STRING
logthis $opt "LTC Redhat Satellite: $LTC_SATELLITE"

# Default to current production satellite
[[ "$SATELLITE" == "" ]] && SATELLITE=$LTC_SATELLITE

if [ $action_count -gt 0 ]; then
  PROGRESS="reporting"
fi

if [[ "$ACTION" == "SHOW_SATELLITE" ]]; then
  logthis $opt "-- Show satellite ------------------------------------------------------"
  logthis $opt "result: $LTC_SATELLITE"
  echo $LTC_SATELLITE
  exit 0
fi

## -----------------------------------------------------------------------------
## All actions after this point require authentication
## -----------------------------------------------------------------------------
require_authentication

if [[ "$ACTION" == "LIST_SYSTEMS" ]]; then
  echo -n "Searching for registered systems... "

  do_action LIST_SYSTEMS "$STYPE" "$SATELLITE"
  RET=$?
  logthis -s "-- List systems --------------------------------------------------------"
  logthis -s "return code: $RET"
  logthis -s "result: $STRING"
  check_result LIST_SYSTEMS $RET "$STRING"
  echo
  echo $STRING | tr ' ' '\n' | sort -h | sed "s/^/    /"
  echo
  exit 0
fi

if [[ "$ACTION" == "SHOW_KEY" ]]; then
  echo -n "Searching for an existing activation key... "

  do_action SHOW_KEY "$STYPE" "$SATELLITE"
  RET=$?
  logthis -s "-- Show key ------------------------------------------------------------"
  logthis -s "return code: $RET"
  logthis -s "result: $STRING"
  check_result SHOW_KEY $RET "$STRING"
  echo
  echo $STRING | tr ' ' '\n' | sort -h | sed "s/^/    /"
  echo
  exit 0
fi

if [[ "$ACTION" == "DELETE_SYSTEMS" ]]; then
  echo -n "Deleting registered systems... "

  do_action DELETE_SYSTEMS "$STYPE" "$SATELLITE" "$SYSTEM_TO_DELETE"
  RET=$?
  logthis -s "-- Delete systems ------------------------------------------------------"
  logthis -s "return code: $RET"
  logthis -s "result: $STRING"
  check_result DELETE_SYSTEMS $RET "$STRING"
  echo $STRING | tr ' ' '\n' | sed -e "s/^/    /" -e "s/    OK:/\nDeleted systems:/" -e "s/    KO:/\nUnable to delete systems:/"
  echo
  [[ "$STRING" == *"KO:"* ]] && exit 1
  exit 0
fi

if [[ "$ACTION" == "REGISTER" ]]; then
  process="registration"
  adverb="already"
  proceed="proceed"
elif [[ "$ACTION" == "UNREGISTER" ]]; then
  process="unregister"
  adverb="currently"
  proceed="unregister"
else
  logthis -E "Indeterminate action: $ACTION"
  exit 1
fi

if [[ ! -x /usr/sbin/subscription-manager ]]; then
  logthis $opt "-- subscription-manager ------------------------------------------------"
  logthis $opt "/usr/sbin/subscription-manager is missing"
  logthis rpm -q subscription-manager
  logthis -E "The subscription-manager command can't be found."
  logthis -E "Check with your administrator to install this package: subscription-manager"
  exit 1
fi

logthis -E "Starting the $process process..."

logthis -n "* Performing initial checks... "
logthis -s "-- Checking hostname ---------------------------------------------------"
run_command hostname
if [[ $stdexit -ne 0 ]]; then
  formatted_echo "FAIL" 1
  logthis -E "Failed to find system hostname."
  logthis -E "Please run the command \"hostname\" to set and check the system hostname."
  exit 1
fi
HOSTNAME=$stdout
# get long hostname if -f is available
run_command hostname -f
[[ $stdexit -eq 0 ]] && HOSTNAME=$stdout
logthis $opt "Hostname: $HOSTNAME"
formatted_echo "OK" 0

PROGRESS=$process
CLEAN_ALL=0

logthis -n "* Querying the server certificate... "
CERT_URL="http://$LTC_SATELLITE/pub/katello-ca-consumer-latest.noarch.rpm"
run_command "curl -o /dev/null --silent -Iw %{http_code} ${CERT_URL}"
if [[ $stdexit -ne 0 ]]; then
  formatted_echo "FAIL" 1
  logthis -E "Error $stdexit querying server certificate"
  exit 1
fi
if [[ "$stdout" != "200" ]]; then
  formatted_echo "FAIL" 1
  logthis -E "Unexpected response $stdout querying server certificate"
  exit 1
fi
formatted_echo "OK" 0

## is the system already registered?
logthis $opt "-- Checking if the system is $adverb registered ------------------------"
for ACTIVE_SATELLITE in $ACTIVE_SATELLITES ; do
  logthis -n "* Checking ${ACTIVE_SATELLITE}..."
  do_action LIST_SYSTEMS "$STYPE" "$ACTIVE_SATELLITE"
  logthis -s "registered systems: $STRING"
  if grep -qw $HOSTNAME <<< "$STRING"; then
    if [[ "$ACTION" == "UNREGISTER" ]]; then
      formatted_echo "OK" 0
    else
      formatted_echo "WARN" 2
    fi
    logthis -E "This system is $adverb registered with $ACTIVE_SATELLITE."
    PROCEED="y"
    if [[ "${FTP3FORCE,,}" != "y" ]]; then
      logthis -n "Would you like to $proceed? (y/n): "
      read PROCEED
    fi
    if [[ "${PROCEED,,}" != "y" ]]; then
      logthis -E "Aborting..."
      echo
      exit 1
    fi
    FTP3FORCE="y"
    CLEAN_ALL=1
    # Delete the current system from the satellite
    logthis -n "* Unregistering from $ACTIVE_SATELLITE..."
    logthis -s "Unregistering $HOSTNAME from $ACTIVE_SATELLITE"
    do_action DELETE_SYSTEMS "$STYPE" "$ACTIVE_SATELLITE" "$HOSTNAME"
    RET=$?
    logthis -s "return code: $RET"
    logthis -s "result: $STRING"
    check_result DELETE_SYSTEMS $RET "$STRING"
  else
    formatted_echo "OK" 0
    if [[ "$ACTION" == "UNREGISTER" ]]; then
      CLEAN_ALL=1
      logthis -E "* Current system isn't registered with $ACTIVE_SATELLITE, finalizing"
    else
      logthis $opt "current system isn't registered with $ACTIVE_SATELLITE"
    fi
  fi
done
if [ $CLEAN_ALL -ne 0 ]; then
  # Clean subscription data as status not always relevant
  logthis -n "* Cleaning registration data... "
  logthis -s "-- Cleaning registration data ------------------------------------------"
  logthis subscription-manager unregister
  logthis subscription-manager remove --all
  logthis subscription-manager clean
  logthis yum clean all
  formatted_echo "OK" 0
fi

if [[ "$ACTION" == "UNREGISTER" ]]; then
  logthis -E "Unregister completed"
  PROGRESS="ok"
  exit 0
fi

logthis -n "* Checking the current system... "
logthis -s "-- Checking release, version and arch ----------------------------------"
if [ "$VERREL" == "" ]; then
  ## get the version and release, most likely only works on RHEL
  command='rpm -qf --qf "%{NAME}-%{VERSION}" /etc/redhat-release'
  run_command "$command"
  if [[ $stdexit -ne 0 ]] ; then
    formatted_echo "FAIL" 1
    logthis -E "Failed to find system version and release with the command:"
    logthis -E "    $command" 
    logthis -E "Is this system running Red Hat Enterprise Linux?"
    logthis $opt "Error while calling: rpm -qf --qf \"%{NAME}-%{VERSION}\" /etc/redhat-release"
    logthis $opt "Result: $VERREL"
    exit 1
  fi
  VERREL=$stdout
fi

# Leading word is almost certainly "redhat" but we use a wildcard just in case.
major_minor=${VERREL#*-release-*}
# Older releases might have an embedded "server-" or "workstation-"
major_minor=${major_minor#*-}
major=${major_minor%.*}
minor=${major_minor#*.}
if [[ $major == $major_minor && $minor == $major_minor ]]; then
  major=${major_minor:0:1}
  minor=${major_minor:1}
fi

#  **FIRST DETERMINE IF THIS IS VERSION 8 OR GREATER.  DIFFERENT SPLIT REQUIRED**
## FIRST split something like "redhat-release-8.0" into "8" and "release"
RELEASE=$major
if [[ $RELEASE -ge 8 ]] ; then
#  VERSION=$(echo $VERREL | cut -f2 -d"-")
  VERSION="server"
else
  ## split something like "redhat-release-server-7.1" into "7" and "server"
  RELEASE=$(echo $VERREL | cut -f4 -d"-" | cut -b1)
  VERSION=$(echo $VERREL | cut -f3 -d"-")
fi

VALID=

## verify support for this release and this version
grep -qvw $RELEASE <<< ${SUPPORTED_RELEASES[@]} && VALID=no && logthis $opt "Unknown or unsupported release: $RELEASE"
grep -qvw $VERSION <<< ${SUPPORTED_VERSIONS[@]} && VALID=no && logthis $opt "Unknown or unsupported version: $VERSION"
if [[ -n "$VALID" ]] ; then
  formatted_echo "FAIL" 1
  logthis -E "Unknown or unsupported system version and release: $VERREL"
  logthis -E "$REPORT"
  exit 1
fi

## get the system arch
ARCH=$(uname -m)

## verify support for this arch
grep -qvw $ARCH <<< ${SUPPORTED_ARCHS[@]} && VALID=no && logthis $opt "Unknown or unsupported arch: $ARCH"
[[ "$VERSION" == "client" && "$ARCH" != "x86_64" ]] && VALID=no && logthis $opt "Unsupported combo version+arch: $VERSION+$ARCH"
if [[ -n "$VALID" ]] ; then
  formatted_echo "FAIL" 1
  logthis -E "Unsupported system architecture: $ARCH"
  logthis -E "$REPORT"
  exit 1
fi

## set LABEL
case $ARCH in
  aarch64 )
    if [[ $RELEASE -ge 8 ]] ; then
      LABEL="for-aarch64"
    else
      LABEL="for-arm-64"
    fi
    ;;
  x86_64 )
    if [[ $RELEASE -ge 8 ]] ; then
      LABEL="for-x86_64"
    else
      LABEL="$VERSION"
    fi
    ;;
  ppc64le )
    if [[ $(subscription-manager facts | grep lscpu.model_name | cut -f2 -d' '| cut -f1 -d',') == "POWER9" ]] ; then
      LABEL="for-power-9"
      if [[ $RELEASE -ge 8 ]] ; then
         LABEL="for-ppc64le"
      fi
    elif [[ $RELEASE -ge 8 ]] ; then
      LABEL="for-ppc64le"
    else
      LABEL="for-power-le"
    fi
    ;;
  ppc64 )
    LABEL="for-power"
    if [[ $RELEASE -ge 8 ]] ; then
       LABEL="for-ppc64"
    fi
    ;;
  s390x )
    LABEL="for-system-z"
    if [[ $RELEASE -ge 8 ]] ; then
       LABEL="for-s390x"
    fi
    ;;
esac

formatted_echo "OK" 0
logthis $opt "Detected a RHEL $RELEASE $VERSION on $ARCH, $LABEL"

## system is registered to the old RHN Satellite?
logthis $opt "-- Checking the system is registered to the old RHN --------------------"
if rpm --quiet -q rhn-org-trusted-ssl-cert; then
  echo "This system is registered to the old RHN Satellite." | tee -a $IBM_RHSM_REG_LOG
  echo -n "Would like to proceed and remove current associations? (y/n): "
  read PROCEED

  if [[ "${PROCEED,,}" != "y" ]]; then
    echo "Aborting..."
    echo
    exit 1
  fi
  logthis yum remove rhn-org-trusted-ssl-cert -y
else
  logthis -s "No"
fi

## Force disabling of rhn plugin
if [[ -f /etc/yum/pluginconf.d/rhnplugin.conf ]]; then
  sed -i 's/enabled\ =\ 1/enabled\ =\ 0/g' /etc/yum/pluginconf.d/rhnplugin.conf
fi

logthis -n "* Checking the server certificate... "
logthis -s "-- Checking the certificate --------------------------------------------"
KATELLO_CERT_RPM="katello-ca-consumer-$LTC_SATELLITE"
logthis -s "Consumer certificate RPM name: $KATELLO_CERT_RPM"
run_command rpm --quiet -q $KATELLO_CERT_RPM
if [ $stdexit -ne 0 ]; then
  formatted_echo "Not Installed" 2
  logthis $opt "The server certificate is not installed."

  run_command "rpm -qa | grep katello-ca"
  if [ "$stdout" != "" ]; then
    logthis $opt "* Removing traces of previous certificates."
    run_command rpm -e $stdout
    if [[ $stdexit -eq 0 ]]; then
      run_command rm -fr /etc/pki/consumer/*
    fi
  fi

  logthis -n "* Installing server certificate... "
  run_command rpm -Uv $CERT_URL
  RET=$stdexit
  logthis subscription-manager config
  if [[ $RET -ne 0 ]]; then
    formatted_echo "FAIL" 1
    logthis -E "An error has occurred while trying to install the server certificate."
    logthis -E "$REPORT"
    exit 1
  else
    formatted_echo "OK" 0
  fi
else
  formatted_echo "OK" 0
  logthis $opt "Server certificate is already installed."
fi

## Get activation key
## in case an existing key is not found, a new one will be created.
logthis -n "* Searching for an activation key... "
logthis -s "-- Activation key ------------------------------------------------------"
do_action CREATE_KEY "$STYPE" "$SATELLITE"
ACTIVATION_KEY="$STRING"
RET=$?
logthis -s "return code: $RET"
logthis -s "activation key (or error message): $ACTIVATION_KEY"
logthis -s "(You may copy this activation key for future use)"
check_result CREATE_KEY $RET "$ACTIVATION_KEY"

## system registration
logthis -n "* Registering the system... "
logthis -s "-- Registering the system ----------------------------------------------"
cmd="subscription-manager register --org $ORGANIZATION --activationkey=\"$ACTIVATION_KEY\" --force"
run_command "$cmd"
if [[ $stdexit -ne 0 ]]; then
  formatted_echo "FAIL" 1
  logthis $opt "Registration failed!"
  logthis $opt "Registration stdout: $stdout"
  logthis $opt "Registration stderr: $stderr"
  logthis -E "An error has occurred while trying to register the system.
You may try to register it later using the following command:
  $cmd
$REPORT"
  exit 1
else
  logthis -s "System successfully registered"
  formatted_echo "OK" 0
fi

logthis subscription-manager facts \| egrep "\"distribution|net\""

## Disable all repositories
logthis -n "* Disabling all repositories... "
logthis -s "-- Disabling repositories ----------------------------------------------"
logthis subscription-manager repos --disable="\"*\""
if [[ $? -ne 0 ]]; then
  formatted_echo "FAIL" 1
  logthis subscription-manager repos --list \| grep "\"Repo ID:\""
else
  formatted_echo "OK" 0
fi

## Enable RHEL repositories
logthis -E "* Enabling RHEL $RELEASE repositories"
logthis $opt "-- list of current repos available --"
logthis subscription-manager repos --list
logthis $opt "-- Enabling repositories -----------------------------------------------"
[[ $LABEL == "for-power-9" ]] && extra="" || extra="supplementary"
[[ $(subscription-manager repos --list |grep rhel-$RELEASE-$LABEL-extras-rpms) ]] && extra="$extra extras"

if [[ $RELEASE -ge 8 ]] ; then
 for repo in appstream supplementary baseos; do
   REPO="rhel-$RELEASE-$LABEL-$repo-rpms"
   logthis -n "    ${REPO}... "
   logthis subscription-manager repos --enable=$REPO
   [[ $? -eq 0 ]] && formatted_echo "OK" 0 || formatted_echo "FAIL" 1
 done
 REPOS="codeready-builder-for-rhel-$RELEASE-$ARCH-rpms"
 REPOS+=" ${ORGANIZATION}_EPEL_epel${RELEASE}-$ARCH"
 for REPO in $REPOS; do
   logthis -n "    ${REPO}... "
   logthis subscription-manager repos --enable=$REPO
   [[ $? -eq 0 ]] && formatted_echo "OK" 0 || formatted_echo "FAIL" 1
 done
else
 for REPO in common optional $extra; do
   logthis -n "    ${REPO^}... "
   [[ $REPO != "common" ]] && str="$REPO-" || str=""
   logthis subscription-manager repos --enable=rhel-$RELEASE-$LABEL-${str}rpms
   [[ $? -eq 0 ]] && formatted_echo "OK" 0 || formatted_echo "FAIL" 1
 done
fi

echo
logthis -E "Registration completed!"
logthis -E "Run the following command to display more available repositories:"
logthis -E "    subscription-manager repos --list-disabled | grep \"Repo ID:\""
logthis -E "Any available repository can be enabled with the following command:"
logthis -E "    subscription-manager repos --enable=<Repo-ID>"

PROGRESS="ok"

exit 0
