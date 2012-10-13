/*****************************************************************************
*
*  PROJECT:     Open Faction
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        client/irr/CMeshSkyboxSceneNode.cpp
*  PURPOSE:     Mesh skybox support for Irrlicht (Irrlicht supports only cubes)
*  DEVELOPERS:  Rafal Harabien
*
*****************************************************************************/

#include "irr/CMeshSkyboxSceneNode.h"
#include <algorithm>

using namespace std;

namespace irr
{
namespace scene
{

//! constructor
CMeshSkyboxSceneNode::CMeshSkyboxSceneNode(IMesh* mesh, ISceneNode* parent, ISceneManager* mgr, s32 id)
: IMeshSceneNode(parent, mgr, id), Mesh(0),
	ReadOnlyMaterials(false)
{
    setAutomaticCulling(scene::EAC_OFF);
	setMesh(mesh);
}


//! destructor
CMeshSkyboxSceneNode::~CMeshSkyboxSceneNode()
{
	if (Mesh)
		Mesh->drop();
}


//! frame
void CMeshSkyboxSceneNode::OnRegisterSceneNode()
{
	if (IsVisible)
	{
		// register
        SceneManager->registerNodeForRendering(this, scene::ESNRP_SKY_BOX);
	}
	
	ISceneNode::OnRegisterSceneNode();
}


//! renders the node.
void CMeshSkyboxSceneNode::render()
{
	video::IVideoDriver* driver = SceneManager->getVideoDriver();
	scene::ICameraSceneNode* camera = SceneManager->getActiveCamera();

	if (!Mesh || !driver || !camera)
		return;

	driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
	
	// calculate world matrix
    core::matrix4 translate(AbsoluteTransformation);
    translate.setTranslation(camera->getAbsolutePosition());

    // Draw the sky box between the near and far clip plane
    const core::aabbox3d<f32> &Aabb = Mesh->getBoundingBox();
    float fMaxDist = max(Aabb.MinEdge.getLength(), Aabb.MaxEdge.getLength());
    const f32 viewDistance = (0.1f * camera->getNearValue() + 0.9 * camera->getFarValue()) / fMaxDist;
    core::matrix4 scale;
    scale.setScale(core::vector3df(viewDistance, viewDistance, viewDistance));

    driver->setTransform(video::ETS_WORLD, translate * scale);

	// render original meshes
    for (u32 i=0; i<Mesh->getMeshBufferCount(); ++i)
    {
        scene::IMeshBuffer* mb = Mesh->getMeshBuffer(i);
        if (mb)
        {
            const video::SMaterial& material = ReadOnlyMaterials ? mb->getMaterial() : Materials[i];

            driver->setMaterial(material);
            driver->drawMeshBuffer(mb);
        }
	}
}


//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CMeshSkyboxSceneNode::getBoundingBox() const
{
    static core::aabbox3df EmptyAabb;
	return Mesh ? Mesh->getBoundingBox() : EmptyAabb;
}


//! returns the material based on the zero based index i. To get the amount
//! of materials used by this scene node, use getMaterialCount().
//! This function is needed for inserting the node into the scene hierarchy on a
//! optimal position for minimizing renderstate changes, but can also be used
//! to directly modify the material of a scene node.
video::SMaterial& CMeshSkyboxSceneNode::getMaterial(u32 i)
{
	if (Mesh && ReadOnlyMaterials && i<Mesh->getMeshBufferCount())
	{
		ReadOnlyMaterial = Mesh->getMeshBuffer(i)->getMaterial();
		return ReadOnlyMaterial;
	}

	if (i >= Materials.size())
		return ISceneNode::getMaterial(i);

	return Materials[i];
}


//! returns amount of materials used by this scene node.
u32 CMeshSkyboxSceneNode::getMaterialCount() const
{
	if (Mesh && ReadOnlyMaterials)
		return Mesh->getMeshBufferCount();

	return Materials.size();
}


//! Sets a new mesh
void CMeshSkyboxSceneNode::setMesh(IMesh* mesh)
{
	if (mesh)
	{
		mesh->grab();
		if (Mesh)
			Mesh->drop();

		Mesh = mesh;
		copyMaterials();
	}
}


void CMeshSkyboxSceneNode::copyMaterials()
{
	Materials.clear();

	if (Mesh)
	{
		video::SMaterial mat;

		for (u32 i=0; i<Mesh->getMeshBufferCount(); ++i)
		{
			IMeshBuffer* mb = Mesh->getMeshBuffer(i);
			if (mb)
				mat = mb->getMaterial();

			Materials.push_back(mat);
		}
	}
}


//! Writes attributes of the scene node.
void CMeshSkyboxSceneNode::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
{
	IMeshSceneNode::serializeAttributes(out, options);

	out->addString("Mesh", SceneManager->getMeshCache()->getMeshName(Mesh).getPath().c_str());
	out->addBool("ReadOnlyMaterials", ReadOnlyMaterials);
}


//! Reads attributes of the scene node.
void CMeshSkyboxSceneNode::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	io::path oldMeshStr = SceneManager->getMeshCache()->getMeshName(Mesh);
	io::path newMeshStr = in->getAttributeAsString("Mesh");
	ReadOnlyMaterials = in->getAttributeAsBool("ReadOnlyMaterials");

	if (newMeshStr != "" && oldMeshStr != newMeshStr)
	{
		IMesh* newMesh = 0;
		IAnimatedMesh* newAnimatedMesh = SceneManager->getMesh(newMeshStr);

		if (newAnimatedMesh)
			newMesh = newAnimatedMesh->getMesh(0);

		if (newMesh)
			setMesh(newMesh);
	}

	// optional attribute to assign the hint to the whole mesh
	if (in->existsAttribute("HardwareMappingHint") &&
		in->existsAttribute("HardwareMappingBufferType"))
	{
		scene::E_HARDWARE_MAPPING mapping = scene::EHM_NEVER;
		scene::E_BUFFER_TYPE bufferType = scene::EBT_NONE;

		core::stringc smapping = in->getAttributeAsString("HardwareMappingHint");
		if (smapping.equals_ignore_case("static"))
			mapping = scene::EHM_STATIC;
		else if (smapping.equals_ignore_case("dynamic"))
			mapping = scene::EHM_DYNAMIC;
		else if (smapping.equals_ignore_case("stream"))
			mapping = scene::EHM_STREAM;

		core::stringc sbufferType = in->getAttributeAsString("HardwareMappingBufferType");
		if (sbufferType.equals_ignore_case("vertex"))
			bufferType = scene::EBT_VERTEX;
		else if (sbufferType.equals_ignore_case("index"))
			bufferType = scene::EBT_INDEX;
		else if (sbufferType.equals_ignore_case("vertexindex"))
			bufferType = scene::EBT_VERTEX_AND_INDEX;

		IMesh* mesh = getMesh();
		if (mesh)
			mesh->setHardwareMappingHint(mapping, bufferType);
	}

	IMeshSceneNode::deserializeAttributes(in, options);
}


//! Sets if the scene node should not copy the materials of the mesh but use them in a read only style.
/* In this way it is possible to change the materials a mesh causing all mesh scene nodes
referencing this mesh to change too. */
void CMeshSkyboxSceneNode::setReadOnlyMaterials(bool readonly)
{
	ReadOnlyMaterials = readonly;
}


//! Returns if the scene node should not copy the materials of the mesh but use them in a read only style
bool CMeshSkyboxSceneNode::isReadOnlyMaterials() const
{
	return ReadOnlyMaterials;
}


//! Creates a clone of this scene node and its children.
ISceneNode* CMeshSkyboxSceneNode::clone(ISceneNode* newParent, ISceneManager* newManager)
{
	if (!newParent)
		newParent = Parent;
	if (!newManager)
		newManager = SceneManager;

	CMeshSkyboxSceneNode* nb = new CMeshSkyboxSceneNode(Mesh, newParent,
		newManager, ID);

	nb->cloneMembers(this, newManager);
	nb->ReadOnlyMaterials = ReadOnlyMaterials;
	nb->Materials = Materials;

	if (newParent)
		nb->drop();
	return nb;
}

} // scene
} // irr
