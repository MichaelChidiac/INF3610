@firstSubstring:
					lwi r10, r1, $0		# r10 = 1er chiffre du nombre �l�ments
					nop
					subi r10, r10, $48	# nombre �l�ments - 48 (ASCII vers d�cimal)
					nop
					muli r10, r10, $10	# multiplication par 10 (dizaines)
					lwi r9, r1, $1		# r9 = 2e chiffre du nombre d'�l�ments
					nop
					subi r9, r9, $48	# nombre �l�ments - 48 (ASCII vers d�cimal)
					nop
					add r10, r10, r9	# r10 = nombre d'�l�ments
					swi r10, r6, $8192 	# envoie nombre �l�ments � CoPro
					addi r1, r2, $1 	# r1 = 1 (pour l'incr�ment de l'adresse de lecture)
					addi r2, r3, $2 	# r2 = compteur
					addi r4, r3, $2		# r4 = 2 �tat de fin de recherche par CoPro
					addi r5, r10, $0	# r5= nombre de lettres dans le mots
					addi r11, r3, $3	# r11 = 3 �tat de fin Win par CoPro
					addi r12, r3, $4	# r12 = 4 �tat de fin Lose par CoPro
@loadFirst:	
					lw r3, r2, r1		# lecture dans le m�m. de donn�es	
					swi r3, r6, $8193  	# �criture au CoPro
					add r2, r2, r1		# compteur + 1
					subi r10, r10, $1	# nombre �l�ments - 1
					nop
					bnei r10, @loadFirst		# branchement
@secondSubstring:
					add r2, r2, r1		# on saute l'espace
					nop
					lw r10, r2, r1		# r10 = 1er chiffre du nombre �l�ments
					nop
					subi r10, r10, $48	# nombre �l�ments - 48 (ASCII vers d�cimal)
					nop
					swi r10, r6, $8192 	# envoie nombre �l�ments � CoPro
					addi r2, r2, $3
					nop
					nop
@waiting:
					lwi r7, r6, $8194 	# chargement �tat CoPro
					nop
					nop
					cmp r8, r7, r11		# comp r11 � 2
					nop
					beqi r8, @affWin	# La partie a ete gagnee
					cmp r8, r7, r12		# comp r12 � 2
					nop
					beqi r8, @affLose	# La partie a ete perdue
					sub r10, r10, r10		#Loop sur la longueur du mot
					addi r10, r5, $0
@affLoop:	
					lwi r0, r6, $8195	# chargement du r�sultat de la recherche
					swi r0, r6,	$4095 	# envoi de la valeur � la Console	
					subi r10, r10, $1	# nombre �l�ments - 1
					nop
					bnei r10, @affLoop		# branchement
					lwi r0, r6, $8196	# chargement du r�sultat de la recherche
					swi r0, r6,	$4096 	# envoi de la valeur � la Console
					nop
					bnei r5, @waiting					
@affWin:	
					swi r0, r6,	$4097 	# adresse pour win
					nop
					ret
@affLose:	
					swi r0, r6,	$4098 	# adresse pour lose
					nop
					ret