/*
 * Skin.cpp
 *
 *  Created on: Oct 8, 2011
 *      Author: mluffel
 */

#include "Body/Skin.h"

namespace Body
{


SkinBinding::SkinBinding() {
	for(int i = 0; i < MAX_BINDINGS; i++) {
		index[i] = -1;
		weight[i] = 0;
	}
}

//void Skin::addParentJoint(int joint_id) {
	//limb_map.insert(std::make_pair(joint_id, num_bones));
	//num_bones++;
//}

void Skin::setNumPoints(int num_points) {
	bindings.resize(num_points);
}

void Skin::addPointToBone(int point_index, int bone_index, float weight) {
	if(bone_index < 0) return; // nothing
	int i = 0;
	for(; i < MAX_BINDINGS; i++) {
		if(bindings[point_index].index[i] == -1) { // an unused slot
			bindings[point_index].index[i] = bone_index;
			bindings[point_index].weight[i] = weight;
			break;
		}
	}
	assert(i < MAX_BINDINGS); // otherwise, too many bindings for this point
}

// TODO: use eigen instead
float dot(float *A, float *B) {
	return A[0]*B[0] + A[1]*B[1] + A[2]*B[2];
}

void transpose(float *src, float *dst) {
	dst[0] = src[0]; dst[1] = src[3]; dst[2] = src[6];
	dst[3] = src[1]; dst[4] = src[4]; dst[5] = src[7];
	dst[6] = src[2]; dst[7] = src[5]; dst[8] = src[8];
}

void localize(float *pos, float *orientation, const pcl::PointXYZRGB *input, pcl::PointXYZ *output) {
	output->x = input->x - pos[0];
	output->y = input->y - pos[1];
	output->z = input->z - pos[2];
	
	// matrix multiply
	output->x = dot(&orientation[0], (float*)output);
	output->y = dot(&orientation[3], (float*)output);
	output->z = dot(&orientation[6], (float*)output);
}

void globalize(Body::Skeleton::JointPose *joint, float weight, pcl::PointXYZ *local, pcl::PointXYZRGB *output) {
	float *position = (float*)&joint->position.position;
	float *orientation = joint->orientation.orientation.elements;
	output->x += weight*( position[0]/1000. + dot(&orientation[0], (float*)local));
	output->y += weight*(-position[1]/1000. + dot(&orientation[3], (float*)local));
	output->z += weight*( position[2]/1000. + dot(&orientation[6], (float*)local));
}

void Skin::bind(pcl::PointCloud<pcl::PointXYZRGB>::ConstPtr all_points, const Body::Skeleton::Pose::Ptr bind_pose) {
	int num_points = all_points->size();
	input_points = all_points;
	// allocate space for localized points
	for(int i = 0; i < MAX_BINDINGS; i++) {
		bound_points[i] = pcl::PointCloud<pcl::PointXYZ>::Ptr(new pcl::PointCloud<pcl::PointXYZ>());
		bound_points[i]->resize(num_points);
	}
	// put bone data in arrays
	float positions[SKELETON_NUMBER_OF_JOINTS][3];
	float orientations[SKELETON_NUMBER_OF_JOINTS][9];
	//for(NameToIndex::const_iterator it = limb_map.begin(); it != limb_map.end(); ++it) {
	for(int j = FirstJoint; j <= LastJoint; j++) {

		Body::Skeleton::JointPose config = (*bind_pose)[(Joint)j];
		XnVector3D v = config.position.position;
		positions[j][0] = v.X/1000.; positions[j][1] = -v.Y/1000.; positions[j][2] = v.Z/1000.;
		// inverse
		transpose((float*)config.orientation.orientation.elements, orientations[j]);
	}
	// put all points in local coordinate frame
	for(int i = 0; i < num_points; i++) {
		for(int j = 0; j < MAX_BINDINGS; j++) {
			int bone_index = bindings[i].index[j];
			if(bone_index >= 0)
			{
				localize(positions[Skeleton::GetBoneJoints((Bone)bone_index).parent],
						 orientations[Skeleton::GetBoneJoints((Bone)bone_index).parent],
						 &all_points->at(i), &bound_points[j]->at(i));

				// add point to bone-only point cloud
				if(bone_clouds.find(bone_index) == bone_clouds.end())
					bone_clouds[bone_index] = BodyPointCloud::Ptr(new BodyPointCloud());
				bone_clouds[bone_index]->push_back(bound_points[j]->at(i));
			}
		}
	}
}

void Skin::renderPosed(const Body::Skeleton::Pose::Ptr pose) {
	// do some vertex shader setup here
}

ColorCloud::Ptr Skin::pose(const Body::Skeleton::Pose::Ptr pose) const {
	int num_points = input_points->size();
	ColorCloud::Ptr output(new ColorCloud());
	
	output->resize(num_points);
	// set the colors just once
	for(int i = 0; i < num_points; i++) {
		output->at(i).r = input_points->at(i).r;
		output->at(i).g = input_points->at(i).g;
		output->at(i).b = input_points->at(i).b;
	}

	//
	Body::Skeleton::JointPose joints[SKELETON_NUMBER_OF_JOINTS];
	for(int i = 0; i < SKELETON_NUMBER_OF_JOINTS; i++) {
		joints[i].position.position.X = joints[i].position.position.Y = joints[i].position.position.Z = 0;
	}
	//std::cout << "limb_map:";
	//for(NameToIndex::const_iterator it = limb_map.begin(); it != limb_map.end(); ++it) {
	for(int j = FirstJoint; j <= LastJoint; j++) {
		//std::cout << j << " -> " << j
		//		<< " (" << (*pose)[(Joint)j].position.position.X
		//		<< " " << (*pose)[(Joint)j].position.position.Y
		//		<< " " << (*pose)[(Joint)j].position.position.Z << ")\n";

		joints[j] = (*pose)[(Joint)j];
	}
	
	// put all points in global coordinate frame
	for(int i = 0; i < num_points; i++) {
		// clear values
		pcl::PointXYZRGB& p = output->at(i);
		p.x = p.y = p.z = 0;
		for(int j = 0; j < MAX_BINDINGS; j++) { // all contributions from each binding
			int index = bindings[i].index[j];
			if(index >= 0)
			{
				float weight = bindings[i].weight[j];
				globalize(&joints[Skeleton::GetBoneJoints((Bone)index).parent], weight, &bound_points[j]->at(i), &p);
			}
		}
	}
	
	return output;
}

Skin::Skin() /*: num_bones(0)*/ {}
}
/**
 * We allocate an output cloud in pose, and localize points in bind.
 * Both are wrapped in boost::shared_ptr, so (I think) we don't need to deallocate them directly.
 */
