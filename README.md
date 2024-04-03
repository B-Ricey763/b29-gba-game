# B29 Superfortress 
Reminiscent of the classic flash game, TU-95, B29 Superfotress is a GBA game with a plane that has to complete a mission or else, FAIL.

## How to play
You start on the runway, motionless. Use `Up` to accelerate and then pitch up with `Left`. As a bomber, the takeoff is one of the hardest and most dangerous parts of the mission, so you will probably fail on the first couple attempts. A couple tips:
- Don't pitch up immediately, it slows you down! Wait until you get up to speed, which happens...
- When the plane reaches takeoff speed it starts shifting to the right of the screen, wait for that moment and then pitch up.
- Keep on pitching up until you reach a high enough altitude.
- Keep on trying! It still takes me a couple tries to get off the ground.

`Right` allows you to pitch down. Maintain your speed and height, and watch the distance indicator in the top left of your screen, that is the distance to your target that you have to bomb. Once you are close enough, the V2 launch site will come into view. Once you are over it, press `Button A` (`Z` on your keyboard) to drop the bomb. If your aim is true, you will have succeeded!

If you crash, you get a lose screen message that tells you the distance you were away from the target (state from the previous screen, extra credit??). 
If you either miss the bomb target or don't drop a bomg at all, you get a failed scren. 

## Project structure
The state machine is abstracted out into a folder, `states/` which has a couple files that each have responsibility over each state. Every stateful function returns the next state to go to, so it is nice and scalable. If you just want the gameplay logic, look at `states/play.c`, specifically the `play()` function, which has all of the logic for user input and plane position calculation. I used a psuedo-physics approach, with velocity that resets every frame (no acceleration), and some magic numbers to get things to play nicely.

I had to do some gnarly DMA stuff to get the image clipping on the left and right side of the screen working in addition to some questionable condiiton logic to use the right function, so see `gba.c` for the special DMA functions and `draw_ground()` in `play.c` for the actual logic. 


