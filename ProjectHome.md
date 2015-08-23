# Introduction #
Open Faction is an independent project to rewrite the Red Faction engine from scratch. It makes use of RF map, texture and sound files so a legal copy of game is needed to use OF. At the moment only the server is usable. Client is in development and needs much more time.
I am actually the main developer. NoCigar helps me from time to time.

# Why did I make it? #
I know Red Faction is nearly a dead game, but I like it and have enjoyed playing it for many years. This project will give me much experience in games development and is also a lot of fun.

# Why not PF? #
  * it seems that Digi and Trotskie are reluctant to share their sources with anyone
  * I have found PF to be unstable (PF 3 often crashes for me)
  * PF is only a patch written in Assembly
  * writing a game from scratch allows me to make everything in my own way instead of hacking Volition's code

# Features #
Right now Open Faction server is capable of:
  * loading levels (it doesn't check filename prefix, so single player maps like L1S1 can be loaded too)
  * loading game items
  * allowing players to pick items up
  * all game modes (DM, TeamDM, CTF) are supported
  * triggers work
  * continuous damage events work
  * players can shoot but collisions are not finished yet, because I have to reverse engineer the Red Faction models and geometry. Currently instead of using real geometry clutter and entities the game uses cylinders and spheres.