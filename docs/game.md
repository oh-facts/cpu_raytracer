<!--

High level
----
~ 2 - 3 hrs of gameplay for someone who has not played before. I don't want to spend too much time on this. I will calculate this by watching other people play. I will sell this based on how much time I invested. I value my skills at 100$/hr (severely understating so I am affordable). I expect to sell 1,000,000 copies. So, I will spend 1000 hrs at most in this. So I will need to sell this at 10$ to break even. And 10.01$ to make a profit. The math is correct for my constants and I pulled the constants out of my ass.


----

3. The puzzle in question is one where the tv asks to have all his pixels returned and promises to  give you a special pixel so you don't have to constantly move. Now, in the third stage of the snake phase, you grow smaller. But its not enough to completely die. So you need to eat yourself and then die. The number of pixels that spawn in this stage will be always be less than your size and I hope the player discovers that eating himself makes him smaller.
So, since now you can get very small even after eating the pixels, it doesn't make sense that you should get the message that you're connecting very fast.
Idea: I make it so that you will connect after a certain amount of time always. The bigger you are, the lesser time it will take. But I mean worst case scenario it could take 10 minutes. Best case scenario, 3 seconds.
So, the dead pixel goes like this. After you collect 3 and lose 3 of your own.
It progresses to the outro after which you get a "special pixel" and the snake arc ends. Now, if you weren't able to kill yourself after 3 pixels, nothing happens. You just waste time in obscurity. special pixel is pink because television loves you now.
There are bugs in the logic. It works fine otherwise. Just playtest and find. The puzzle is in a good place I think.
The bugs were fixed. I need to add the "thank you + pink pixel stage"
Complete the dialogue bit too. Also, story wise, sim should stop. tv thanks you but then complains its going to take an absurd time to load. He lets you explore the world while you're loading. He also tells you that there will be a very small window when the thing opens again. As pink pixel you can explore the world. calculaing the window should be trivial. Look at length of full wait, look at shortest wait. Calculate. Maybe I will leave some hint. But you can explore the world until time passes by. You can do quests for people. And they will give you items. Also, make it so that each dialogue in pixel dead stage comes one at a time as you eat them. So that one doesn't eat all of them accidentally. Doesn't solve the main problem since player can always just spam collect them by moving erratically. Ideal soln would be to provide a transcript to refer to. I do this for the ... blah blah.
I think a much better soln is used to have all the dialogue print beforehand. That works perfectly I think. Also since old noise was very distracting. 

Perhaps we can do something like this. Dead pixels are just black squares that get rendererd randomly (fixed positions). Also now i do the candy colored background "static" since the noise is very distracting


8.work on doing audio properly. this is neccessary to pause audio.
12. loading bar should be identical to snake.

14. Don't call them waves. Call them stages.

15. Put all snake level data inside snake and pass snake around. Don't do this now but once you start working enough on other levels, consider doing it. Actually this just reads as stupid because my game struct will always fit into cache unless its very very very massive. As of now, it is not very very very massive. Do it only if naming variables becomes pain. Just checked L1 is 512KB on my comp. Holy fuck.

18. Parse ttf files and render text yourself

Things I added
// 3/15/24
1. snakeself collision
2. If he does, he cuts off those bits. This will be used in a puzzle for times when you need to be a specific size.
4.Saving thing (Dialogue + functionality)
5.Restarting
7.remove all global data that shouldn't be global data

// dont remember when
9.remove local_persist variables. that is also global state.
10. Separating game api code from actual game code (Not too inclined since I don't see myself making another game with this engine)
11. Consider using stb_tt to parse fonts and render them. I would like this so I can play full screen (Can't play fullscreen if I am using the title for text)
13. Fix annoying shit where the Trying to connect prints twice when you first enter the SNAKE_WAVE_START stage

16. text rendering (with sdl_ttf)


// 3/19
6.hot reloading. I want to avoid this since sdl doesn't have functions to do this in a cross-platform way.
19. parsing and rendering bitmaps

// 3/20
17. screen resizing. All resolutions, pixel perfect, fullscreen support

--!>


