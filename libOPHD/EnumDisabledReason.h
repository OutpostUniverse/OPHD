#pragma once


enum class DisabledReason
{
	None, /**< Not Disabled, default reason. */

	Chap, /**< Requires atmosphere, no atmosphere available. */
	Disconnected, /**< Not connected to Command Center */
	Energy, /**< Not enough Energy to operate. */
	Population, /**< Insufficient workers or scientists (or both) */
	RefinedResources, /**< Insufficient mined and refined resources */
	StructuralIntegrity /**< Structural integrity out of operating tolerances (damaged structure) */
};
