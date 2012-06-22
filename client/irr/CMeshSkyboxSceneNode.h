/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        client/irr/CMeshSkyboxSceneNode.h
*  PURPOSE:     Mesh skybox support for Irrlicht (Irrlicht supports only cubes)
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#ifndef CIRRMESHSKYBOXSCENENODE_H
#define CIRRMESHSKYBOXSCENENODE_H

#include <irrlicht.h>

namespace irr
{
namespace scene
{

class CMeshSkyboxSceneNode: public IMeshSceneNode
{
    public:

        //! constructor
		CMeshSkyboxSceneNode(IMesh* mesh, ISceneNode* parent, ISceneManager* mgr, s32 id);

		//! destructor
		virtual ~CMeshSkyboxSceneNode();

		//! frame
		virtual void OnRegisterSceneNode();

		//! renders the node.
		virtual void render();

		//! returns the axis aligned bounding box of this node
		virtual const core::aabbox3d<f32>& getBoundingBox() const;

		//! returns the material based on the zero based index i. To get the amount
		//! of materials used by this scene node, use getMaterialCount().
		//! This function is needed for inserting the node into the scene hirachy on a
		//! optimal position for minimizing renderstate changes, but can also be used
		//! to directly modify the material of a scene node.
		virtual video::SMaterial& getMaterial(u32 i);
		
		//! returns amount of materials used by this scene node.
		virtual u32 getMaterialCount() const;

		//! Writes attributes of the scene node.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const;

		//! Reads attributes of the scene node.
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0);

		//! Returns type of the scene node
		virtual ESCENE_NODE_TYPE getType() const { return ESNT_MESH; }

		//! Sets a new mesh
		virtual void setMesh(IMesh* mesh);

		//! Returns the current mesh
		virtual IMesh* getMesh(void) { return Mesh; }

		//! Sets if the scene node should not copy the materials of the mesh but use them in a read only style.
		/* In this way it is possible to change the materials a mesh causing all mesh scene nodes 
		referencing this mesh to change too. */
		virtual void setReadOnlyMaterials(bool readonly);

		//! Returns if the scene node should not copy the materials of the mesh but use them in a read only style
		virtual bool isReadOnlyMaterials() const;

		//! Creates a clone of this scene node and its children.
		virtual ISceneNode* clone(ISceneNode* newParent=0, ISceneManager* newManager=0);

	protected:

		void copyMaterials();

		core::array<video::SMaterial> Materials;
		video::SMaterial ReadOnlyMaterial;

		IMesh* Mesh;

		bool ReadOnlyMaterials;
};

} // scene
} // irr

#endif // CIRRMESHSKYBOXSCENENODE_H
