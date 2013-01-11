/*
  Copyright � 2012 The KoRE Project

  This file is part of KoRE.

  KoRE is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  KoRE is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with KoRE.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CORE_INCLUDE_SCENENODECOMPONENT_H_
#define CORE_INCLUDE_SCENENODECOMPONENT_H_

namespace kore {
  class SceneNode;
  class SceneNodeComponent {
  public:
    SceneNodeComponent(void);
  virtual ~SceneNodeComponent(void);
  SceneNode* getNode(void);
  void attachTo(SceneNode* node);
  int64_t getID(void);
  private:
    int64_t _id;
    SceneNode* _node;
  };
};
#endif  // CORE_INCLUDE_SCENENODECOMPONENT_H_
