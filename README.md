# RUNNER GAME 

*Projet en collaboration avec l'ISEN de Lille, proposé comme sujet de projet en fin de première année, dans l'apprentissage de l'Arduino.*

## Objectif

Faire une application interactive avec la carte arduino ISEN de type runner. Cette activité permet de contrôler un personnage avec un unique bouton de saut. Le personnage représente un coureur et reste à la même place sur l’affichage tandis que le paysage défile à l’horizontal. Il faut esquiver les obstacles à l’aide du saut afin de parcourir la plus grande distance possible.

## Matériel

Le matériel nécessaire pour ce projet est intégralement présent sur les cartes ISEN mises à disposition (un bouton poussoir, une led, un écran lcd et un arduino).

## Conseils

Le jeu sera basé sur une boucle contenant une instruction delay(XX). Vous devez associer une seule image (16*2 caractères) par boucle et la succession des boucles donnera l’impression de mouvement. En diminuant la valeur de XX vous pouvez faire accélérer le jeu.

Prévoyez une séquence d’images assez courte pour la montée du personnage et une à plusieurs étapes de suspension avant de commencer à vérifier s’il doit redescendre.Il fautde plusqu’entre deux obstaclesil y ait le temps de faire une séquence de descente plus une séquence de montée complète.

L’écran lcd peut mémoriser jusqu’à 8 caractères modifiables (de 16*5 pixels) numérotés de 0 à 7.Vous pouvez créer un caractère pixel par pixel.

### Mentions Spéciales 

- Wladimir Lucet
- Enguerrand Marquant
- Paul Slosse 