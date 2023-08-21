#ifndef MAGIX3D_EVENTS_H
#define MAGIX3D_EVENTS_H

/**
 * <P>Ce fichier définit des évènement <I>Magix 3D</I>. Ces évènements
 * sont transmis aux observateurs d'entités qui, selon leur nature,
 * effectue une action (raffraîchissement d'un élément de l'IHM,
 * actualisation d'une entité, ...) ou non.
 * </P>
 *
 * \warning	Ces évenements suivent ceux déjà définis dans 
 *			<I>ReferencedObject.h</I> de la bibliothèque <I>TkUtil</I>.
 */

/**
 * Les commandes Magix 3D.
 * <PRE>
 * <I>NEW_COMMAND</I>			: Création d'une commande,<BR>
 * <I>COMMAND_REMOVED</I>		: Suppression d'une commande,<BR>
 * <I>COMMAND_STATE</I>			: Changement d'état d'une commande,<BR>
 * <I>COMMAND_PROGRESSION</I>	: Avancement d'une commande,<BR>
 * <I>COMMAND_STACK</I>			: Modification de la pile d'appel des
 *								  commandes (<I>done</I>, <I>undo</I>,
 *								  <I>redo</I>, ...).
 * </PRE>
 */
#define NEW_COMMAND                  ((unsigned long)100)
#define COMMAND_REMOVED              ((unsigned long)(NEW_COMMAND<<1LL))
#define COMMAND_STATE                ((unsigned long)(COMMAND_REMOVED<<1LL))
#define COMMAND_PROGRESSION          ((unsigned long)(COMMAND_STATE<<1LL))
#define COMMAND_STACK                ((unsigned long)(COMMAND_PROGRESSION<<1LL))

/**
 * La gestion de la sélection.
 * <PRE>
 * <I>ENTITY_ADDED_TO_SELECTION</I>     : Entité ajoutée à la sélection,<BR>
 * <I>ENTITY_REMOVED_FROM_SELECTION</I> : Entité enlevée de la sélection,<BR>
 * <I>SELECTION_MODIFIED</I>            : Sélection modifiée (sans plus de précisions),<BR>
 * <I>SELECTION_CLEARED</I>             : Sélection annulée,<BR>
 * </PRE>
 */
#define ENTITY_ADDED_TO_SELECTION		((unsigned long)(COMMAND_STACK<<1LL))
#define ENTITY_REMOVED_FROM_SELECTION	((unsigned long)(ENTITY_ADDED_TO_SELECTION<<1LL))
#define SELECTION_MODIFIED				((unsigned long)(ENTITY_REMOVED_FROM_SELECTION<<1LL))
#define SELECTION_CLEARED				((unsigned long)(SELECTION_MODIFIED<<1LL))


#endif	// MAGIX3D_EVENTS_H
