#pragma once

#include "StructureManager.h"
#include <NAS2D/Utility.h>


/**
 * Return a reference to the given StructureComponent type belonging to
 * a structure. The structure is assumed to have the given component,
 * and it is an error to try to get a component from a structure that
 * does not have it.
 */
template<typename T>
inline T& getComponent(SKey s)
{
	return NAS2D::Utility<StructureManager>::get().get<T>(s);
}


/**
 * Return a pointer to the given StructureComponent type belonging
 * to a structure, if it has the corresponding component type.
 * Otherwise return nullptr.
 */
template<typename T>
inline T* tryGetComponent(SKey s)
{
	return NAS2D::Utility<StructureManager>::get().tryGet<T>(s);
}


/**
 * Returns a range object that can be used to iterate over all instances of the
 * given StructureComponent type. It is suitable for use in range-based for loops.
 */
template<typename ComponentTy>
const ComponentRange<ComponentTy> enumerateComponent()
{
	return NAS2D::Utility<StructureManager>::get().enumerateComponent<ComponentTy>();
}


/**
 * Common base class for all structure components.
 * Each structure is associated with a set of components that define the functional properties of the structure.
 * A structure either has a given component or not - it can never have multiple instances of the same component type.
 *
 * The StructureComponent base class is abstract in the sense that it cannot be constructed or queried.
 * Component classes deriving from StructureComponent must declare the following field:
 * static constexpr ComponentTypeID componentTypeID = ...;
 */
class StructureComponent
{
private:
	SKey mKey; /**< Key of the structure owning this component. */

protected:
	StructureComponent(SKey key) : mKey(key) {}

public:
	virtual ~StructureComponent() {}

	SKey key() const { return mKey; }

	/**
	 * Obtain a reference to the Structure instance belonging to this structure.
	 * It is guaranteed to exist.
	 */
	Structure& structure() const { return getComponent<Structure>(mKey); }

	/**
	 * Convenience function to get a different component type from the same structure.
	 */
	template<typename T> T& get() { return getComponent<T>(mKey); }

	/**
	 * Convenience function to get a different component type from the same structure.
	 */
	template<typename T> T* tryGet() { return tryGetComponent<T>(mKey); }
};
