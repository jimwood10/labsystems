#!/usr/bin/env python
import json
import sys

# open the audit log file
fName="/gpfs/fs1/.audit_log/SpectrumScale_150_13023685172280762785_1750413175_FSYS_fs1_audit/auditLogFile.latest_alloc-node1"
try:
  fn = open(fName, 'r')
except IOError:
  print ('ERROR: Opening file', fName, '\n')
  sys.exit(1)

# read the file line by line and display the relevant events: Event-type, event-time, file-owner, file-path
i = 0
for line in fn:
  obj = json.loads(line)
  if i == 0:
    print ("\n{:10} {:26} {:6} {:6} {}".format("Event", "Event-time", "Owner","User", "Path"))
    print ('---------------------------------------------------------------------------------------------')
  print ("{:10} {:26} {:6} {:6} {}".format(obj["event"], obj["eventTime"], obj["ownerUserId"], obj["clientUserId"], obj["path"]))
  i = i + 1
