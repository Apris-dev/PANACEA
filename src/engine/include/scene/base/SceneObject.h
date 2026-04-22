#pragma once

#include "rendercore/Instancer.h"
#include "basic/core/Object.h"
#include "basic/core/Class.h"

class CSceneObject : public SObject {
public:
	REGISTER_CLASS(CSceneObject, SObject)

public:

	CSceneObject() = default;

	CSceneObject(const std::string& inName): mName(inName) {}

	no_discard virtual Matrix4f getTransformMatrix() const = 0;

	virtual COutputArchive& save(COutputArchive& inArchive) const {
		inArchive << mName;
		return inArchive;
	}

	virtual CInputArchive& load(CInputArchive& inArchive) {
		inArchive >> mName;
		return inArchive;
	}

	friend COutputArchive& operator<<(COutputArchive& inArchive, const CSceneObject& inValue) {
		inValue.save(inArchive);
		return inArchive;
	}

	friend CInputArchive& operator>>(CInputArchive& inArchive, CSceneObject& inValue) {
		inValue.load(inArchive);
		return inArchive;
	}

	virtual void onMoved() {}

	std::string mName{"Scene Object"};

};

class CViewportObject : public CSceneObject, public THierarchy<CViewportObject> {

	REGISTER_CLASS(CViewportObject, CSceneObject)

public:

	CViewportObject(): CSceneObject("Viewport Object") {}

	no_discard EXPORT virtual Matrix4f getTransformMatrix() const override;

	friend COutputArchive& operator<<(COutputArchive& inArchive, const CViewportObject& inValue) {
		inValue.save(inArchive);
		inArchive << static_cast<const THierarchy&>(inValue);
		return inArchive;
	}

	friend CInputArchive& operator>>(CInputArchive& inArchive, CViewportObject& inValue) {
		inValue.load(inArchive);
		inArchive >> static_cast<THierarchy&>(inValue);
		return inArchive;
	}

	virtual COutputArchive& save(COutputArchive& inArchive) const override {
		CSceneObject::save(inArchive);
		inArchive << m_Transform;
		return inArchive;
	}

	virtual CInputArchive& load(CInputArchive& inArchive) override {
		CSceneObject::load(inArchive);
		inArchive >> m_Transform;
		return inArchive;
	}

	Vector2f getOrigin() const { return m_Transform.getOrigin(); }
	Vector2f getPosition() const { return m_Transform.getPosition(); }
	float getRotation() const { return m_Transform.getRotation(); }
	Vector2f getScale() const { return m_Transform.getScale(); }

	void setOrigin(Vector2f inOrigin) {
		m_Transform.setOrigin(inOrigin);
		onMoved();
	}

	void setPosition(Vector2f inPosition) {
		m_Transform.setPosition(inPosition);
		onMoved();
	}

	void setRotation(const float inRotation) {
		m_Transform.setRotation(inRotation);
		onMoved();
	}

	//TODO: 2d viewport scale should be based on relative coordinates
	void setScale(Vector2f inScale) {
		//const Vector2f screenSize = CEngineViewport::get().mExtent;
		//m_Transform.setScale(inScale * screenSize);
		m_Transform.setScale(inScale);
		onMoved();
	}

private:

	Transform2f m_Transform;
};

class CWorldObject : public CSceneObject, public THierarchy<CWorldObject> {
public:

	REGISTER_CLASS(CWorldObject, CSceneObject)

public:

	CWorldObject(): CSceneObject("World Object") {}

	no_discard virtual Matrix4f getTransformMatrix() const override {
		return m_Transform.toMatrix();
	}

	friend COutputArchive& operator<<(COutputArchive& inArchive, const CWorldObject& inValue) {
		inValue.save(inArchive);
		inArchive << static_cast<const THierarchy&>(inValue);
		return inArchive;
	}

	friend CInputArchive& operator>>(CInputArchive& inArchive, CWorldObject& inValue) {
		inValue.load(inArchive);
		inArchive >> static_cast<THierarchy&>(inValue);
		return inArchive;
	}

	virtual COutputArchive& save(COutputArchive& inArchive) const override {
		CSceneObject::save(inArchive);
		inArchive << m_Transform;
		return inArchive;
	}

	virtual CInputArchive& load(CInputArchive& inArchive) override {
		CSceneObject::load(inArchive);
		inArchive >> m_Transform;
		return inArchive;
	}

	Vector3f getPosition() const { return m_Transform.getPosition(); }
	Vector3f getRotation() const { return m_Transform.getRotation(); }
	Vector3f getScale() const { return m_Transform.getScale(); }

	void setPosition(const Vector3f inPosition) {
		m_Transform.setPosition(inPosition);
		onMoved();
	}

	void setRotation(const Vector3f inRotation) {
		m_Transform.setRotation(inRotation);
		onMoved();
	}

	void setScale(const Vector3f inScale) {
		m_Transform.setScale(inScale);
		onMoved();
	}

private:

	Transform3f m_Transform;

};