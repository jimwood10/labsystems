#!/usr/bin/env python3
import sys
import argparse
from fyreapi import FyreAPI
#from gooey import *

#Sample usage of fyre api and interface

#@Gooey(default_size=(1024,1024), program_name='FYRE CLIENT', footer_bg_color='#ffffff', terminal_panel_color='#ffffff',terminal_font_color='#000000')
def main():

    parser = argparse.ArgumentParser()

    #parser = GooeyParser()
    parser.add_argument("-user_id", nargs="?", help="Your FYRE userid", type=str)
    parser.add_argument("-user_api_key",nargs="?", help="Your FYRE api key", type=str)
    parser.add_argument("--prefix", nargs="?",help="prefix that you would like to have pre-pended to all of your nodes names.", type=str)
    parser.add_argument("--instance_type", nargs="?",help="either: virtual_server OR physical_server", type=str)
    parser.add_argument("--platform", nargs="?",help="Platform type x, p or z", type=str)
    parser.add_argument("--nodename", nargs="?",help="If your cluster_prefix is foo and your node name is bar, and your quantity is 3, you will get the hostname 'foo-bar-3.fyre.ibm.com' for one of your nodes.", type=str)
    parser.add_argument("--node_count", nargs="?",choices=[i for i in range(1, 33)],help="How many nodes you need", type=int)
    parser.add_argument("--cpu_num", nargs="?", help="Cpus per node", type=int)
    parser.add_argument("--node_mem", nargs="?",help="How many GB of memory per node", choices=[i for i in range(1,33)],type=int)
    parser.add_argument("--nsd_sizes", nargs="+", help="enter nsd sizes in GB, so for 2 nsds that are 10GB and 20GB you would enter like --nsd_sizes 10 20")
    parser.add_argument("--osvers", nargs="?",help="such as 'Redhat 7.9', 'Redhat 8.1', 'Redhat 8.2', 'Redhat 8.3', etc", type=str)
    parser.add_argument("--status", dest="status", type=str, help="check status of a node")
    parser.add_argument("--file", dest="fyre_config_file", nargs="?", help="get arguments from file")#, widget="FileChooser")
    parser.add_argument("--forcepublic", action="store_true")

    args = parser.parse_args()

    if args.user_id and args.user_api_key and not args.fyre_config_file and not args.prefix and args.status is None:
        parser.error("only user_id and api_key specified, need more arguments")

    if(args.user_id is None or args.user_api_key is None) and args.fyre_config_file is None:
        print("Must include user_id and user_api_key")
        sys.exit(0)

    if args.fyre_config_file is not None:
        api = FyreAPI(fyre_config_file=args.fyre_config_file, force_public=args.forcepublic)
        api.build_complex_cluster()
        sys.exit(0)

    if args.status is not None:
        api = FyreAPI(userid=args.user_id, api_key=args.user_api_key, fyre_config_file=args.fyre_config_file, request_id=args.status)
        print(api.check_node_status())
        sys.exit(0)

    #if we aren't passing config file or querying status use CLI to build
    if args.fyre_config_file is None or args.status is not None and args.prefix is None:
        api = FyreAPI(
            userid=args.user_id,
            api_key=args.user_api_key,
            cluster_prefix=args.prefix,
            instance_type=args.instance_type,
            nodename=args.nodename,
            platform=args.platform,
            node_num=args.node_count,
            cpu_num=args.cpu_num,
            node_mem=args.node_mem,
            os_version=args.osvers,
            nsd_sizes=args.nsd_sizes,
            force_public=args.forcepublic
            )
        api.build_complex_cluster()
        sys.exit(0)
if __name__ == "__main__":
    main()
