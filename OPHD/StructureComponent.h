#pragma once

#include <NAS2D/Utility.h>

class Structure;
class StructureManager;
class StructureComponent;


/**
 * Key type for identifying a specific structure instance.
 * The key for any given structure is guaranteed to remain unchanged for the lifetime of the structure.
 * The key for any given structure is guaranteed to be unique during the lifetime of the structure.
 *
 * Every structure has a Structure instance. The Structure pointer is used as key to allow O(1) access
 * to the Structure instance. This is an internal detail and should not be relied upon by code handling the key.
 */
class SKey
{
private:
	Structure* mStructure;
public:
	SKey(Structure* structure) : mStructure(structure) {}

	/** Comparison operators to allow using this type in ordered containers such as maps and sets. */
	bool operator<(const SKey& rhs) const { return mStructure < rhs.mStructure; }

	/** Do not call this function directly. It is intended only for GetComponent/TryGetComponent. */
	Structure* getInternal() { return mStructure; }
};


/**
 * Return a reference to the given StructureComponent type belonging to
 * a structure. The structure is assumed to have the given component,
 * and it is an error to try to get a component from a structure that
 * does not have it.
 */
template<typename T>
inline T& GetComponent(SKey s)
{
	return NAS2D::Utility<StructureManager>::get().get<T>(s);
}

/**
 * Return a reference to the Structure type belonging to a structure.
 * This allows writing code that's agnostic to the SKey type.
 */
template<>
inline Structure& GetComponent<Structure>(SKey s)
{
	return *s.getInternal();
}


/**
 * Return a pointer to the given StructureComponent type belonging
 * to a structure, if it has the corresponding component type.
 * Otherwise return nullptr.
 */
template<typename T>
inline T* TryGetComponent(SKey s)
{
	return NAS2D::Utility<StructureManager>::get().tryGet<T>(s);
}

/**
 * Return a pointer to the Structure type belonging to a structure.
 * This allows writing code that's agnostic to the SKey type.
 */
template<>
inline Structure* TryGetComponent<Structure>(SKey s)
{
	return s.getInternal();
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
public:
	typedef int ComponentTypeID; // TODO: replace by enum class.

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
	Structure& structure() const { return GetComponent<Structure>(mKey); }

	/**
	 * Convenience function to get a different component type from the same structure.
	 */
	template<typename T> T& Get() { return GetComponent<T>(mKey); }

	/**
	 * Convenience function to get a different component type from the same structure.
	 */
	template<typename T> T* TryGet() { return TryGetComponent<T>(mKey); }
};
