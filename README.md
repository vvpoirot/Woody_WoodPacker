# 42-WoodyWoodpacker

> vpoirot, bgaertne

> Projet decouverte des "packers".

## Fonctionnement d'un Packer

Un `Packer`est un outils utilisé pour compresser et chiffrer des exécutables.<br>
Un programme passé dans un packer est réduit en taille, et rendu illisible sans la clé de déchiffrement.<br>
Lorsque le programme est exécuté, il est d'abord chargé en mémoire sous sa forme compressée et chiffrée.<br>
Juste avant l'exécution, il est décompressé et déchiffré en mémoire pour etre executé normalement.<br>

## Utilité d'un Packer

On peux utiliser un packer pour plusieurs raisons; légitimes ou non.<br>
Il reduit la taille des fichiers exécutables pour la distribution de logiciel, et on peux se servir de l'obfurcation du code pour empecher l'analyse ou la modification non autorisées de logiciel (Protection de la propriété intellectuelle). Les logiciels malveillant peuvent neanmois se servir de packer pour obfusquer leurs codes et eviter la detection antivirus.


