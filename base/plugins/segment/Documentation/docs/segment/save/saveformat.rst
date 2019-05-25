.. index:: Format de sauvegarde

Format de sauvegarde
====================

Le format de sauvegarde, en JSON, possède la structure générale
suivante:

.. code:: js

   {
       "Document": {
           "Process": {
               "Scenes": [ ... ],
               "Transitions": [ ... ]
           }
       }
   }

Des clefs JSON supplémentaires peuvent être présentes dans les fichiers
de sauvegarde, mais elles ne sont utiles qu’à l’éditeur.

Les deux objets principaux sont les scènes et les transitions.

Scène
-----

Une scène possède le format suivant:

.. code:: js

   {
     // Identifant unique de la scène (pour transitions)
     "Path": "/SEGMentDocument.1/SEGMentProcess.0/Scene.1",
     
     // Label affiché
     "Metadata": {
         "Label": "TableauDia1"
     },
     
     // Position absolue de la scène dans l'éditeur
     "Rect": [
         2656.3397062431295,
         2722.3061676621014,
         640,
         512.5
     ],
     
     
     // Type de scène
     // 0: scène normale
     // 1: scène initiale
     // 2: scène finale
     // 3: scène d'échec (game over)
     "SceneType": 0,
     
     // Texte affiché lors de l'entrée dans la scène
     "StartText": "",
     "Image": "Ressources/Scenes/TableauDia1.png",
     
     // Son d'ambiance
     "Ambience": {
         "Path": "",
         "Range": 0, 
         "Volume": 0
     },
     
     // Objets présents dans la scène
     "Objects": [ ], // Images simples
     "Gifs": [ ], // Gifs
     "ClickAreas": [ ], // Zones de clic
     "BackClickAreas": [ ], // Zones de retour en arrière
     "TextAreas": [ ], // Zones de texte
   }

Images
------

Images simples
~~~~~~~~~~~~~~

.. code:: js

   {
     // Identifiant unique de l'objet (pour transitions)
     "Path": "/SEGMentDocument.1/SEGMentProcess.0/Scene.1/SimpleObject.0",
     "Image": "Ressources/Objects/clef.png",
     
     // Les positions et tailles sont relatives à la scène parente.
     // L'origine du repère est le coin supérieur gauche de la scène.
     // L'axe vertical est pointé vers le bas.
     "Pos": [ 0.5, 0.7 ],
     "Size": [ 0.3, 0.3 ],
     
     // Son associé à l'objet
     "Sound": {
         "Path": "",
         "Range": 0,
         "Volume": 0
     }
   }

Gifs
~~~~

Comme pour les images simples, avec les champs additionnels suivants :

.. code:: js

   // Frame par défaut de la gif
   "Default": 0,

   // Attributs des frames de la gif
   // 0: Ignorée
   // 1: Active
   // 2: Victorieuse
   "Frames": [
       0, 
       0,
       0
   ]

Zones spéciales
---------------

Zones de clic simple
~~~~~~~~~~~~~~~~~~~~

.. code:: js

   {
       "Path": "/SEGMentDocument.1/SEGMentProcess.0/Scene.1/ClickArea.0",
       "Pos": [ 0.20, 0.22 ],
       "Size": [ 0.3, 0.4 ],
       "Sound": {
           "Path": "",
           "Range": 0,
           "Volume": 0
       }
   }

Zones de retour en arrière
~~~~~~~~~~~~~~~~~~~~~~~~~~

Comme pour les zones de clic simple.

Zones de texte
~~~~~~~~~~~~~~

Comme pour les zones de clic simple, avec les champs additionnels
suivants :

.. code:: js

   "Text": "Texte associé"
   
   // Behaviour : comportement de la zone de texte.
   // 0: Ajouter
   // 1: Remplacer
   // 2: Effacer
   // 3: Valider
   "Behaviour": 0

Transitions
-----------

.. code:: js

   {
     // Type de fade
     // 0: Immédiat
     // 1: CrossFade
     "Fade": 0,
     
     // Couleur de la transition, forma RGBA entre 0 et 255
     "Color": [ 0, 0, 0, 255 ],
     
     // Son associé à la transition
     "Sound": {
         "Path": "",
         "Range": 0,
         "Volume": 0
     },
     
     // Ici plusieurs possibilités selon les cas :
     // ObjectToScene (image simple vers scène)
     // ClickToScene (zone de clic vers scène)
     // SceneToScene (scène vers scène)
     // GifToScene (gif vers scène, ce cas est-il utile ?)
     
     // Cas simples: ObjectToScene, ClickToScene, GifToScene
     // Par ex. pour une transition d'un objet vers une scène: 
     
     "Transition": {
       "ObjectToScene": {
         // Les identifiants utilisés sont ceux des attributs "Path" des objets
         "From": "/SEGMentDocument.1/SEGMentProcess.0/Scene.0/SimpleObject.0",
         "To": "/SEGMentDocument.1/SEGMentProcess.0/Scene.1"
       }
     }
   }

.. code:: js

   {
     // Cas plus compliqué avec une énigme: scène vers scène
     "Transition": {
       "SceneToScene": {
         // Les identifiants utilisés sont ceux des attributs "Path" des objets
         "From": "/SEGMentDocument.1/SEGMentProcess.0/Scene.0/SimpleObject.0",
         "To": "/SEGMentDocument.1/SEGMentProcess.0/Scene.1"
         "Riddle": { ... }
       }
     }
   }

Énigmes
~~~~~~~

L’objet ``Riddle`` peut être de la forme suivante :

-  Pas d’énigme:

.. code:: js

   "Riddle": {
   }

-  Énigme sur texte:

.. code:: js

   "Riddle": {
     "Text": {
       // Texte attendu pour réussir
       "Expected": "", 
       
       // Text affiché en cas de réussite
       "IfCorrect": "",
       
       // Text affiché en cas d'échec
       "IfWrong": ""
     }
   }

-  Énigme sur gif:

.. code:: js

   "Riddle": {
     "Gif": { }
   }

-  Énigme sur puzzle:

.. code:: js

   "Riddle": {
     "Puzzle": { }
   }
