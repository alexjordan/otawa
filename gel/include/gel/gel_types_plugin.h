/*
 * $Id$
 *
 * This file is part of the GEL library.
 * Copyright (c) 2005-08, IRIT- UPS
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef GEL_TYPES_PLUGIN_H_
#define GEL_TYPES_PLUGIN_H_

#include <gel/common.h>
#include <gel/image.h>
#include <gel/types.h>

__BEGIN_DECLS

/* sys_plugin_t type */
typedef struct sys_plugin_s {
	u8_t system;
	u8_t version;
	void *dlh;
	gel_block_t **(*plug_image_map) (gel_env_t *env, gel_file_t *f,
			gel_file_t **tab, u32_t *num);
	int (*plug_image_load) (gel_env_t *env, gel_block_t **tab, u32_t num,
			vaddr_t* stack_pointer);
} sys_plugin_t;


/* arch_plugin_t type */
typedef struct arch_plugin_s {
	u16_t machine;
	u8_t version;
	u8_t stack_align;
	void *dlh;
	int (*plug_do_reloc)(gel_image_t *r, int flags);
	u32_t align;
	u32_t psize;
} arch_plugin_t;


/**
 * Structure de donnee decrivant une plateforme. (type de CPU et autre
 * parametres, endianness, .. )
 */
struct gel_platform_s {
  u16_t machine; /**< Machine tel que defini dans la specification ELF */
  u8_t system; /**< Systeme d'exploitation */
  int endianness; /**< Little endian ou big endian */
  int float_type; /**< Type de representation de flottant */
  int refcount; /**< Nombre de fois que cette plateforme est liee a un objet */
  sys_plugin_t *sys; /**< Structure d'information du plug-in systeme */
  arch_plugin_t *arch; /**< Structure d'information du plug-in d'archi */
};

/**
 * Structure decrivant l'image d'un fichier ELF en memoire (contrairement
 * a image_t il ne contient qu'un fichier ELF et non un processus entier.)
 * Il contient les donnees de tout les segments du fichier ainsi que les
 * informations de mapping adresse_reelle <==> adresse_virtuelle
 */
struct gel_block_s {
  gel_file_t *container; /**< Fichier ELF */
  u8_t *data; /**< Donnees du fichier ELF charge */
  raddr_t base_raddr; /**< Adresse de base reelle (0 si pas charge) */
  vaddr_t base_vaddr; /**< Adresse de base virtuelle (0 si pas charge) */
  vaddr_t exec_begin; /**<Adresse virtuelle du debut de la premiere portion executable */
  u32_t exec_size; /**< Taille de l'image de ce fichier seulement */
  gel_memory_cluster_t *cluster; /**< cluster dans lequel est implante ce block
                                      de programme (si on a demande une
                                      clusterisation de la memoire sinon NULL)*/
  gel_platform_t *plat; /**< Plateforme associee */
};

__END_DECLS

#endif /*GEL_TYPES_PLUGIN_H_*/
