/*
 Copyright (c) 2012 The VCT Project

  This file is part of VoxelConeTracing and is an implementation of
  "Interactive Indirect Illumination Using Voxel Cone Tracing" by Crassin et al

  VoxelConeTracing is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  VoxelConeTracing is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with VoxelConeTracing.  If not, see <http://www.gnu.org/licenses/>.
*/

/*!
* \author Dominik Lazarek (dominik.lazarek@gmail.com)
* \author Andreas Weinmann (andy.weinmann@gmail.com)
*/


#include "VoxelConeTracing/Octree Mipmap/BorderTransferPass.h"
#include "KoRE\RenderManager.h"
#include "VoxelConeTracing/Scene/VCTscene.h"
#include "Kore\Operations\Operations.h"

BorderTransferPass::
  BorderTransferPass(VCTscene* vctScene, EBrickPoolAttributes eBrickPool, EThreadMode eThreadMode,
                     int level, kore::EOperationExecutionType executionType) {
  using namespace kore;

  _name = std::string("BorderTransfer (level: ").append(std::to_string(level).append(")"));
  _useGPUProfiling = vctScene->getUseGPUprofiling();

  this->setExecutionType(executionType);
  _level = level;

  _axisX = 0;
  _axisY = 1;
  _axisZ = 2;
  _axisX_neg = 3;
  _axisY_neg = 4;
  _axisZ_neg = 5;
  _shdAxisX.type = GL_UNSIGNED_INT;
  _shdAxisX.data = &_axisX;
  _shdAxisY.type = GL_UNSIGNED_INT;
  _shdAxisY.data = &_axisY;
  _shdAxisZ.type = GL_UNSIGNED_INT;
  _shdAxisZ.data = &_axisZ;
  _shdAxisX_neg.type = GL_UNSIGNED_INT;
  _shdAxisX_neg.data = &_axisX_neg;
  _shdAxisY_neg.type = GL_UNSIGNED_INT;
  _shdAxisY_neg.data = &_axisY_neg;
  _shdAxisZ_neg.type = GL_UNSIGNED_INT;
  _shdAxisZ_neg.data = &_axisZ_neg;

  _shdLevel.component = NULL;
  _shdLevel.name = "Level";
  _shdLevel.size = 1;
  _shdLevel.type = GL_INT;
  _shdLevel.data = &_level;
    
  if (eThreadMode == THREAD_MODE_COMPLETE) {
    _shader.loadShader("./assets/shader/BorderTransfer.shader",
                       GL_VERTEX_SHADER, "#define THREAD_MODE 0\n\n");
  } else if (eThreadMode == THREAD_MODE_LIGHT) {
    _shader.loadShader("./assets/shader/BorderTransfer.shader",
                        GL_VERTEX_SHADER, "#define THREAD_MODE 1\n\n");
  }
  
  _shader.setName("BorderTransfer shader");
  _shader.init();

  this->setShaderProgram(&_shader);
  addStartupOperation(new ColorMaskOp(glm::bvec4(false, false, false, false)));

  if (eThreadMode == THREAD_MODE_LIGHT) {
    addStartupOperation(
      new kore::BindBuffer(GL_DRAW_INDIRECT_BUFFER,
      vctScene->getThreadBuf_nodeMap(level)->getHandle()));

    addStartupOperation(new BindTexture(
      vctScene->getShdLightNodeMapSampler(),
      _shader.getUniform("nodeMap")));

    addStartupOperation(new BindUniform(vctScene->getShdNodeMapOffsets(),
      _shader.getUniform("nodeMapOffset[0]")));

    addStartupOperation(new BindUniform(vctScene->getShdNodeMapSizes(),
      _shader.getUniform("nodeMapSize[0]")));

  } else {
    addStartupOperation(
      new kore::BindBuffer(GL_DRAW_INDIRECT_BUFFER,
      vctScene->getNodePool()->getDenseThreadBuf(_level)->getHandle()));

    addStartupOperation(
      new kore::BindUniform(vctScene->getNodePool()->getShdNumLevels(),
      _shader.getUniform("numLevels")));

    addStartupOperation(
      new kore::BindTexture(
      vctScene->getNodePool()->getShdLevelAddressBufferSampler(),
      _shader.getUniform("levelAddressBuffer")));
  }

  addStartupOperation(new BindUniform(&_shdLevel, _shader.getUniform("level")));
  
  addStartupOperation(new BindTexture(
    vctScene->getNodePool()->getShdNodePoolSampler(COLOR),
    _shader.getUniform("nodePool_color")));

  addStartupOperation(new BindImageTexture(
    vctScene->getBrickPool()->getShdBrickPool(eBrickPool),
    _shader.getUniform("brickPool_value")));

    // X Axis ADD
  addStartupOperation(new BindUniform(&_shdAxisX, _shader.getUniform("axis")));
  addStartupOperation(new BindTexture(
    vctScene->getNodePool()->getShdNodePoolSampler(NEIGHBOUR_X),
    _shader.getUniform("nodePool_Neighbour")));

  addStartupOperation(new DrawIndirectOp(GL_POINTS, 0));
  addStartupOperation(new MemoryBarrierOp(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));
  //////////////////////////////////////////////////////////////////////////
  
  // Y Axis ADD
  addStartupOperation(new BindUniform(&_shdAxisY, _shader.getUniform("axis")));
  addStartupOperation(new BindTexture(
    vctScene->getNodePool()->getShdNodePoolSampler(NEIGHBOUR_Y),
    _shader.getUniform("nodePool_Neighbour")));

  addStartupOperation(new DrawIndirectOp(GL_POINTS, 0));
  addStartupOperation(new MemoryBarrierOp(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));
  //////////////////////////////////////////////////////////////////////////
   
  // Z Axis ADD
  addStartupOperation(new BindUniform(&_shdAxisZ, _shader.getUniform("axis")));
  addStartupOperation(new BindTexture(
    vctScene->getNodePool()->getShdNodePoolSampler(NEIGHBOUR_Z),
    _shader.getUniform("nodePool_Neighbour")));

  addStartupOperation(new DrawIndirectOp(GL_POINTS, 0));
  addStartupOperation(new MemoryBarrierOp(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));
  //////////////////////////////////////////////////////////////////////////

}

BorderTransferPass::~BorderTransferPass(void) {

}
