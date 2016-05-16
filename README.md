## Usage
1. Open a terminal and in the root of this project, run: `make clean`, `make`, `make run`.
2. In another terminal: `cd ./client` and run: `./client.bin`

Note: you may open as many clients as you want.

## Commands to use in the chatroom:

Commands are to be executed in the following way: `/command param1 param2`

* `/login` 
	* Params: `username` `password` 

* `/register`
	* Params: `username` `password` 

* `/delete`
	* Params: `username` 

* `/logout`

* `/change_password`
	* Params: `old_password` `new_password` 

* `/change_privileges`
	* Params: `username` `new_priveleges`

* `/change_color`
	* Params: `color` It's an integer between 0 and 6 
	
* `/kick`
	* Params: `username` `reason`

* `/ban`
	* Params: `username` `reason` 

* `/get_online_users`