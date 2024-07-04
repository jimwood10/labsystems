Sample Ansible files for a few odds and ends

A quick check of the hosts file can be:

   ansible all -i inventory.txt -m ping

To run a playbook, eg:
   ansible-playbook playbook_yum.yml -i inventory.txt
