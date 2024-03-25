[ ] Logging
[x] create a log file
[ ] After dropping the root privilege, append the command line arguments followed by a newline character to the per-user log file

[ ] Dropping the root privilege
[ ] During the execution of mac, drop the root privilege when you no longer need it => After read or write command.

[ ] Interface
[ ] Read: level of user is specified in mac.policy file && following read-down policy
[ ] Write: level of user is specified in mac.policy file && following write-up policy
[ ] followed by a newline chaaracter
[ ] ACCESS DENIED
[ ] Append the <data> even after ACCESS DENIED

[x] Read the mac.policy file with the root privilege
[x] Find and compare the user is specified in mac.policy file
[x] if then, get the security clearance level of the user
[x] else, print ACCESS DENIED & exit
[x] Dropping the root privilege
[x] Validate read-down or write-up with the level and command
[x] Perform the command if following read-down or write-up policy
[x] else, print ACCESS DENIED

* Questions
[ ] Should I consider any other error case regarding to the commands except for read and write?