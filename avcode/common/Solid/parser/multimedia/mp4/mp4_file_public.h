/**
 * @file mp4_file_public.h   MP4 File Public
 * @author ÎÌÑå <robert_3000@sina.com>
 * @version 0.1
 * @date 2012.03.28
 *
 *
 */

#ifndef __MP4_FILE_PUBLIC_H__
#define __MP4_FILE_PUBLIC_H__

#include <public/gen_int.h>
#include <public/gen_allocator.h>
#include "../iso/iso_media_file.h"

/** File Box */
typedef struct tag_mp4_file_box
{
	/** offset */
	int64   offset;

	/** format */
	int32   format;

	/** payload */
	union
	{
		/** box */
		ISO_BOX box;
		/** box64 */
		ISO_BOX64 box64;
		/** full box */
		ISO_FULL_BOX full_box;
		/** full box64 */
		ISO_FULL_BOX64 full_box64;

	} payload;

} MP4_FILE_BOX, *PMP4_FILE_BOX;

/** File Box Node */
typedef struct tag_mp4_file_box_node
{
	/** tree */
	/** parent */
	struct tag_mp4_file_box_node*  p_parent;
	/** prev sibling */
	struct tag_mp4_file_box_node*  p_prev_sibling;
	/** next sibling */
	struct tag_mp4_file_box_node*  p_next_sibling;
	/** first child */
	struct tag_mp4_file_box_node*  p_first_child;
	/** last child */
	struct tag_mp4_file_box_node*  p_last_child;

	/** list */
	/** prev node */
	struct tag_mp4_file_box_node*  p_prev;
	/** next node */
	struct tag_mp4_file_box_node*  p_next;

	/** box */
	MP4_FILE_BOX box;

} MP4_FILE_BOX_NODE, *PMP4_FILE_BOX_NODE;

/** File Tree */
typedef struct tag_mp4_file_tree
{
	/** allocator */
	GEN_ALLOCATOR allocator;

	/** init */
	int32  b_init;
	/** tree size */
	int32  tree_size;

	/** root pointer */
	MP4_FILE_BOX_NODE* p_root;
	/** root */
	MP4_FILE_BOX_NODE root;

	/** head pointer */
	MP4_FILE_BOX_NODE* p_head;
	/** rear pointer */
	MP4_FILE_BOX_NODE* p_rear;
	/** head */
	MP4_FILE_BOX_NODE  head;
	/** rear */
	MP4_FILE_BOX_NODE  rear;

} MP4_FILE_TREE, *PMP4_FILE_TREE;


#endif ///__MP4_FILE_PUBLIC_H__
