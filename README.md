# TarkovGameInfo

Program that utilizes h33p's vmread library to read/write windows processes on a Linux KVM. https://github.com/h33p/vmread

Specifically, the program reads memory for the game, "Escape From Tarkov." 
It finds the Game Object Manager's Address, loops through the Active Objects, looking for the Local Game World - 
which its address can be used with other offsets to find other information about the current game world. This implementation 
prints the amount of players in the game on the patch .12.6
