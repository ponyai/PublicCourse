// Copyright @2018 Pony AI Inc. All rights reserved.

#pragma once

namespace gl {

// All the classes that implement 'Bind' and 'Unbind' can use 'BindingGuard' class to be bound and
// unbound automatically(according to the guard's life-cycle).
class BindingInterface {
 public:
  virtual void Bind() = 0;
  virtual void Unbind() = 0;
};

// This is a simple implementation of scope guard. We might need a more general one and introduce
// it to others in the future.
//
// Usage:
//    {
//      BindingGuard guard(&fbo);  // Here's when "fbo->Bind()" is called.
//      DoSomething();
//    }  // Here's when fbo->Unbind() is called.
//
class BindingGuard {
 public:
  // The object will be bound once the binding guard is created.
  explicit BindingGuard(BindingInterface* object) : object_(object) { object_->Bind(); }

  // Unbind the object when the binding guard instance is deleted.
  ~BindingGuard() { object_->Unbind(); }

 private:
  BindingInterface* object_ = nullptr;
};

}  // namespace gl
