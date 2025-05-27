echo "Populating inventory.txt from the /etc/hosts file"

echo "#hosts" > inventory.txt
grep fyre /etc/hosts | awk '{print $2}' >> inventory.txt

echo "Finished"
cat inventory.txt
ansible all -i inventory.txt -m ping
