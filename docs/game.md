<!--

3. The puzzle in question is one where the tv asks to have all his pixels returned and promises to  give you a special pixel so you don't have to constantly move. Now, in the third stage of the snake phase, you grow smaller. But its not enough to completely die. So you need to eat yourself and then die. The number of pixels that spawn in this stage will be always be less than your size and I hope the player discovers that eating himself makes him smaller.
So, since now you can get very small even after eating the pixels, it doesn't make sense that you should get the message that you're connecting very fast.
Idea: I make it so that you will connect after a certain amount of time always. The bigger you are, the lesser time it will take. But I mean worst case scenario it could take 10 minutes. Best case scenario, 3 seconds.
 	
6.hot reloading
8.work on doing audio properly. this is neccessary to pause audio.
9.remove local_persist variables. that is also global state


Things I added
// 3/15/24
1. snakeself collision
2. If he does, he cuts off those bits. This will be used in a puzzle for times when you need to be a specific size.
4.Saving thing (Dialogue + functionality)
5.Restarting
7.remove all global data that shouldn't be global data

--!>