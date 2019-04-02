Hello,

If you need that tools, its mostly because you want the world seed of your world
So to get it you need 3 things: first you need to collect all the pillar heights
in the end, you need to follow the order in main.html (just double click on it,
it will open in your browser as a webpage) 
Then you need the coordinates of a few structures, Some are easier to get like village
and temples but do notice that you need to use a mix of the structure and the rarer
they are (also the bigger) the better, i would recommend using ocean monument and 
end cities.
How do you know which chunk to take into account for the structure? 
For small structures like witch hut/igloo/shipwreck/temples, its quite simple its 
the chunks where they are (pls dont use structure on a chunk border, they are likely
misplaced) For Ocean monument, it used to be till 1.13 the center chunk, now since 1.13 
since there is 4 center chunks, you need to take the south east one.
For mansion you need to take the entrance chunk on the right taking part of the stair.
For end city take the chunk in the middle with the maximum of the base of the city.
For village, you need to take the chunk with the well in the middle.
For Ocean ruins, you need to take the chunk with the central chest (not obvious)
For hidden treasure, you need to take the chunk it is in (not reliable)
For slime chunks, take the chunk.

Then you need some biome coordinates, i would recommend to use really rare biomes like
variants or mushroom islands...

If you need any help, just contact me on discord: Neil#4879

The sources are available here: https://github.com/hube12/MinecraftSeedCrackingC

I support 1.9 to 1.14 currently but some issue may arise, also i didnt merge the gui yet 
coz i need to redo the windows process handler.
There is prebuild binary on the github but you can also do `cmake -f CMakeLists.txt` and then `make`
It will crap out the repo but will craft in src a binary.
So far on windows you need the cygwin dll, but since windows is POSIX i will get rid of them
at next update (i promise it will be sooner than last time).

Cheers neil
