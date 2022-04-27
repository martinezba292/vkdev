#ifndef __VK_INTERFACE_H__
#define __VK_INTERFACE_H__ 1

namespace vkdev {
  enum InterfaceType {
    InterfaceType_NONE = -1,
    InterfaceType_VertexBuffers = 0 //{uint64 first_bind, uint32 n_elements}
  };
  
  class VlkInterface {
    public:
      VlkInterface(){ interface_ = InterfaceType_NONE; }
      ~VlkInterface(){}
  
      const InterfaceType& getInterface() const { return interface_; }
  
    protected:
      InterfaceType interface_;
  };
}

#endif