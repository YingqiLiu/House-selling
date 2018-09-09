Yingqi Liu     5200537875

My codes are Agent.cpp, Buyer.cpp, and Seller.cpp. 
Agent communicates with other two code, gets and sends data, calculates the final result.
Buyer communicates with Agent and campares the received data with its own requairment to find out which house it wants.
Seller sends its house's information to Agent and get result from Agent.

1. Run "makefile.sh" to compile the codes, if it doesn't work, enter: "g++ -o Agent Agent.cpp -lsocket -lnsl"  "g++ -o Buyer Buyer.cpp -lsocket -lnsl" and "g++ -o Seller Seller.cpp -lsocket -lnsl" to complie them.
2. enter "./Agent &" to run Agent in the background, enter "./Seller &" to run Seller in the background, after it, enter "./Buyer" within 8 seconds, otherwise it will fail.

The only thing you need to pay attention is that you need to enter "./Buyer" within 8 seconds after enter "./Seller &".

I only use some sentences from Internet to run some functions, but I don't remember where I found them. They are all very short sentences.