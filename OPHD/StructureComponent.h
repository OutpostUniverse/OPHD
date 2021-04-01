#pragma once

#include "StructureManager.h"

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
