#!/usr/bin/env python3
import os
import sys
from time import sleep
import subprocess
import json
import logging
import configparser
import requests
import urllib3
from Crypto.PublicKey import RSA


#disables InsecureRequestWarning on output
urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)

logging.basicConfig()
logger = logging.getLogger(__name__)

# when new OS gets put on fyre it should be tested and added here
VALID_OS = [
    "Windows 2022",
    "Windows 2019",
    "Windows 2016",
    "Windows 11",
    "Windows 10",
    "RedHat 9.2",
    "RedHat 9.4",
    "RedHat 8.10",
    "RedHat 8.8",
    "CentOS Stream 9",
    "SLES 15.5",
    "SLES 15.6",
    "Ubuntu 22.04",
    "Ubuntu 20.04",
    "Ubuntu 24.04"
]


API_VERSION = "1"

class  FyreAPI:
    """
    Class uses to communicate with fyre rest api for building complex clusters and querying status

    Attributes
    ----------
    userid : str
        Fyre API username (can be found at https://fyre.ibm.com/account)
    api_key : str
        Fyre API KEY found at https://fyre.ibm.com/account
    public_key (optional): str
        public ssh key that you would like to have injected into each of the nodes in
        your cluster by using a public ssh key, you will be able to log into your nodes from the
        server you specify without using a password
    cluster_prefix : str
        prefix that you would like to have pre-pended to all of your nodes names.
        For example: if you used "foobar" for your prefix, you would get a hostnames like
        "foobar-master1.fyre.ibm.com"
    instance_type : str
        Specifies whether you would like physical (bare metal) or virtual (VM) nodes.
        Options:
            physical_server
            virtual_server
    nodename : str
        name of the nodes this name will be appended to your cluster prefix value with a number
        appended based on the quantity you specify. if your cluster_prefix is foo and your node
        name is bar, and your quantity is 3, you will get the hostname "foo-bar-3.fyre.ibm.com"
        for one of your nodes.
    platform : str
        specifies which platform to use. Use "x" for xSeries (x86_64),
        "p" for Power (Power8, ppc64le) or "z" for zLinux
    node_num (optional): str
        number of nodes with the current name and configuration
        value between 1 and 100 Defaults to 1 if not specified.
    cpu_num : str
        number of CPU's per node (value between 1 and 16)
    node_mem : str
         amount of RAM per node in GB (value between 1 and 32)
    os_version : str
        OS version such as 'RedHat 8.2', 'RedHat 7.8', SLES 12SP5, etc
        current OS's https://fyre.ibm.com/help#fyre-os
    nsd_sizes: list
        list of nsd sizes
    request_id : str
        used for querying status of an existing requestid, requestid gets shows after sending
        POST request
    fyre_config_file : file
        rather than inputting each paramater, you can use a file in .ini format
        example is on the github repository

    Methods
    -------
    get_ssh_key()
        queries the users public key if it is found at ~/.ssh/id_rsa.pub, if not
        it creates one and uses that.
    parse_config_file(file)
        parses the config file and updates the json thats sent via POST to fyre
        returns: boolean
    build_complex_cluster()
        sends the json to fyre to build the cluster
    check_node_status()
        queries the status of a cluster that has been sent to fyre to be built
    """

    def __init__ (
        self,
        userid=None,
        api_key=None,
        public_key=None,
        cluster_prefix=None,
        instance_type=None,
        nodename=None,
        platform=None,
        node_num=1,
        cpu_num=1,
        node_mem=1,
        os_version=None,
        request_id = None,
        fyre_config_file=None,
        nsd_sizes = None,
        force_public = False #secret switch for all public IPs
        ):
        self.session = requests.session()
        self._userid = userid
        self._api_key = api_key
        self.public_key = public_key
        self.cluster_prefix = cluster_prefix
        self.instance_type = instance_type
        self.platform = platform
        self.nodename = nodename
        self.node_mem = node_mem
        self.os_version = os_version
        self.node_num = node_num
        self.cpu_num = cpu_num
        self.request_id = request_id
        self.fyre_config_file = fyre_config_file
        self.nsd_sizes = nsd_sizes
        self.force_public = force_public

        self. _base_url = f"https://api.fyre.ibm.com/rest/v{API_VERSION}/"
        self.headers = {
            'Content-type': 'application/json',
            'Accept': 'text/plain'
        }
        #config file parse
        if self.fyre_config_file is not None:
            if not self.parse_config_file(self.fyre_config_file):
                raise FileNotFoundError("File doesn't exist")

        if self.public_key is None:
            self.get_ssh_key()


        if self.request_id is None:
            try:
                self._check_inputs()
            except NameError as e:
                print(e)
                exit(1)

            #only build json if not querying status
            if self.force_public is False:
                self.build_complex_json = {
                            "fyre": {
                                "creds": {
                                    "username": self._userid,
                                    "api_key": self._api_key,
                                    "public_key": self.public_key
                                }
                            },
                            "cluster_prefix": self.cluster_prefix,
                            "clusterconfig" : {
                                "instance_type" : self.instance_type,
                                "platform" : self.platform,
                                "user_comment" : "test system"
                            },
                            self.cluster_prefix : [
                                {
                                    "name" : self.nodename + "-mgr",
                                    "count" : 1,
                                    "cpu" : int(self.cpu_num),
                                    "memory" : int(self.node_mem),
                                    "os" : self.os_version,
                                    "publicvlan" :"y",
                                    "privatevlan" : "y",
                                    "additional_disks" : ([{"size": int(i)} for i in self.nsd_sizes])
                                },
                                {
                                    "name" : self.nodename,
                                    "count" : int(self.node_num) - 1,
                                    "cpu" : int(self.cpu_num),
                                    "memory" : int(self.node_mem),
                                    "os" : self.os_version,
                                    "publicvlan" :"n",
                                    "privatevlan" : "y",
                                    "additional_disks" : ([{"size": int(i)} for i in self.nsd_sizes])
                                }
                            ]
                        }
            else:
                self.build_complex_json = {
                    "fyre": {
                            "creds": {
                                "username": self._userid,
                                "api_key": self._api_key,
                                "public_key": self.public_key
                            }
                        },
                        "cluster_prefix": self.cluster_prefix,
                        "clusterconfig" : {
                            "instance_type" : self.instance_type,
                            "platform" : self.platform,
                            "user_comment" : "test system"
                        },
                        self.cluster_prefix : [
                            {
                                "name" : self.nodename,
                                "count" : int(self.node_num),
                                "cpu" : int(self.cpu_num),
                                "memory" : int(self.node_mem),
                                "os" : self.os_version,
                                "publicvlan" :"y",
                                "privatevlan" : "y",
                                "additional_disks" : ([{"size": int(i)} for i in self.nsd_sizes])
                            }
                        ]
                    }

            # The RTP site (the default for most users on the Scale team) does not have zLinux
            # If z was the requested platform, force the site to SVL
            if self.platform == "z":
                self.build_complex_json["site"] = "svl"



    def get_ssh_key(self):
        """
        Gets users public key if it is found at ~/.ssh/id_rsa.pub and uses that so we can
        pass that when building cluster so user will not need a password.

        If key is not found we will generate it automatically and write the public key file
        to ~/.ssh/id_rsa.pub and use this key so user will be able to access fyre nodes without
        a password. If there is an error with creating and/or saving the key we will continue
        as this is not a required argument for fyre.
        """

        try:
            with open(os.path.join(os.path.expanduser('~'),'.ssh/id_rsa.pub'), 'rb') as keyfile:
                self.public_key = keyfile.read().decode('utf-8').strip()
        except PermissionError:
            print("Cannot access ~/.ssh/id_rsa.pub due to permissions!")
            print("Run:")
            print("\tchmod 700 ~/.ssh")
            print("\tchmod 644 ~/.ssh/*.pub")
            print("\tchmod 600 ~/.ssh/id_rsa ~/.ssh/authorized_keys")
            sys.exit(1)
        except FileNotFoundError:
            #generate pub/priv key for user if none
            output = subprocess.run(["ssh-keygen -t  rsa -N '' -f ~/.ssh/id_rsa"], shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            if output.returncode == 0:
                with open(os.path.join(os.path.expanduser('~'),'.ssh/id_rsa.pub'), 'rb') as keyfile:
                    self.public_key = keyfile.read().decode('utf-8').strip()
            else:
                print("Error creating public key!")
                sys.exit(1)


    def parse_config_file(self, fyre_config_file):
        if not os.path.exists(fyre_config_file):
            return False
        fyre_config = configparser.ConfigParser()
        fyre_config.read(fyre_config_file)
        if fyre_config['CREDS']['username'] == "":
            self._userid = os.environ.get('FYRE_USER_ID')
        else:
            self._userid = fyre_config['CREDS']['username']
        if fyre_config['CREDS']['username'] == "":
            self._api_key = os.environ.get('FYRE_API_KEY')
        else:
            self._api_key = fyre_config['CREDS']['api_key']

        if fyre_config['CREDS']['public_key'] == "":
            self.public_key = None
        else:
            self.public_key = fyre_config['CREDS']['public_key']

        self.cluster_prefix = fyre_config['CLUSTER_CONFIG']['cluster_prefix']
        self.instance_type = fyre_config['CLUSTER_CONFIG']['instance_type']
        self.platform = fyre_config['CLUSTER_CONFIG']['platform']
        self.nodename = fyre_config['CLUSTER_CONFIG']['node_name']
        self.node_num = int(fyre_config['CLUSTER_CONFIG']['count'])
        self.cpu_num = int(fyre_config['CLUSTER_CONFIG']['cpu'])
        self.node_mem = int(fyre_config['CLUSTER_CONFIG']['memory'])
        self.os_version = fyre_config['CLUSTER_CONFIG']['os']
        self.nsd_sizes = fyre_config['CLUSTER_CONFIG']['nsd_sizes'].split(",")

        return True

    def _check_inputs(self):
        if self._userid is None:
            raise NameError("userid is not defined")
        if self._api_key is None:
            raise NameError("api_key is not defined")
        #if we are querying status we don't care about below being None
        if self.request_id is None:
            if self.platform not in ["p", "x", "z"]:
                raise NameError(f"platform ({self.platform}) can only be 'x', 'p' or 'z'")
            if self.node_num not in [i for i in range(1, 33)]:
                raise NameError(f"Only 1-32 nodes can be created (requested {self.node_num})")
            if self.cpu_num not in [i for i in range(1, 17)]:
                raise NameError(f"cpu can only be between 1-16 per node (requested {self.cpu_num})")
            if self.node_mem not in [i for i in range(1, 33)]:
                raise NameError(f"node memory must be between 1-32 per node (requested{self.node_mem})")
            if self.os_version not in VALID_OS:
                raise NameError(f"OS version \"{self.os_version}\" is not valid.\nValid values include {', '.join(VALID_OS)}")

    def build_complex_cluster(self):
        operation = "?operation=build"
        build_endpoint = self. _base_url+operation
        response = requests.post(
            build_endpoint,
            data=json.dumps(self.build_complex_json),
            verify=False,
            auth=(self._userid, self._api_key),
            headers=self.headers
            )
        if response.ok:
            #now check to see if its building with/without error
            sleep(5)
            ret = self.check_node_status(response.json()['request_id'])
            try:
                if ret['request'][0]['status'] == "error":
                    print(f"Cluster build FAILURE: {ret['request'][0]['error_details']}")
                else:
                    print(f"The cluster is being built with request id {response.json()['request_id']} go to https://fyre.ibm.com/stacks to get the ip addresses")
            #usually means api key/username is invalid
            except:
                print(ret)
        else:
            print(f"Build cluster POST failed with {response.text}")

    def check_node_status(self, requestid=None):
        if requestid is not None:
            operation = f"?operation=query&request=showrequests&request_id={requestid}"
            status_endpoint = self. _base_url + operation
        else:
            operation = "?operation=query&request=showrequests&request_id="
            status_endpoint = self. _base_url + operation + self.request_id
        response = requests.get(
            status_endpoint,
            verify=False,
            auth=(self._userid, self._api_key),
            headers=self.headers
            )
        return response.json()
