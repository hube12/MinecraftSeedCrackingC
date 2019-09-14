To download the tool go in release: https://github.com/hube12/MinecraftSeedCrackingC/releases/download/2.0/SeedCracking.zip 

I WILL NOT PROVIDE ANY HELP VIA DISCORD ANYMORE (NOR ANY OTHER MEDIUM), ASSUME THAT YOU MADE A MISTAKE WHILE GATHERING THE DATA, COMMON ISSUES ARE:
- taking Chunk cooordinates for both structures and biomes
- taking biomes cooridinates for both structures and biomes
- using XYZ instead of the line looking at block (or the newly block:) while looking down for biomes.
- taking the wrong chunk for ocean monument (its the south east of the 4 center chunk for 1.13+)
- using villages in 1.14+, the center isn't the well anymorz
- taking the wrong end city chunk, its more than 60% of the ground block in the chunk
- NOT READING THE README.TXT GIVEN IN THE RELEASE
- not checking the biomes id with the wiki to see if you filled up correctly the data.txt (aka new names from 1.13 old/new menu)
- running this release on a modified server aka paper/spigot with different world generation, end seed different from overworld or changed salt (there is a gpu version if needed on my github)
- using this tool on a changed seed on non vanilla server (aka end seed will be different from overworld one)
- modifying the data.txt with notepad from windows and not using notepad++ (screw line endings)
- Being generally not cautious with the data gathering.



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
