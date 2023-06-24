#include <Eigen/Eigen>
#include <jjyou/gl/Shader.hpp>
#include <jjyou/gl/Window.hpp>
#include <jjyou/gl/FrameBuffer.hpp>
#include <jjyou/gl/Objects.hpp>
#include <jjyou/io/PlyFile.hpp>
#include <opencv2/opencv.hpp>


struct PointLight {
	bool enabled;
	glm::vec3 lightPosition;
	glm::vec3 lightColor;
	float ambientStrength;
	float specularStrength;
	//attenuation
	float constant;
	float linear;
	float quadratic;
};
struct DirectionalLight {
	bool enabled;
	glm::vec2 lightDirection; //yaw and pitch, in randians
	glm::vec3 lightColor;
	float ambientStrength;
	float specularStrength;
};
struct SpotLight {
	bool enabled;
	glm::vec3 lightPosition;
	glm::vec2 lightDirection; //yaw and pitch, in randians
	float innerCutOff; //in randians
	float outerCutOff; //in randians
	glm::vec3 lightColor;
	float ambientStrength;
	float specularStrength;
	//attenuation
	float constant;
	float linear;
	float quadratic;
};

inline void setLightingUniformVariables(
	const jjyou::gl::Shader& shader,
	bool transformLight,
	const glm::mat4& modelView,
	PointLight pointLight[],
	DirectionalLight directionalLight[],
	SpotLight spotLight[]
) {
	shader.use();
	for (int i = 0; i < 4; i++) {
		shader.setBool(
			"pointLight[" + std::to_string(i) + "].enabled",
			pointLight[i].enabled
		);
		if (transformLight)
			shader.setVec3(
				"pointLight[" + std::to_string(i) + "].lightPosition",
				modelView * glm::vec4(pointLight[i].lightPosition, 1.0)
			);
		else
			shader.setVec3(
				"pointLight[" + std::to_string(i) + "].lightPosition",
				pointLight[i].lightPosition
			);
		shader.setVec3(
			"pointLight[" + std::to_string(i) + "].lightColor",
			pointLight[i].lightColor
		);
		shader.setFloat(
			"pointLight[" + std::to_string(i) + "].ambientStrength",
			pointLight[i].ambientStrength
		);
		shader.setFloat(
			"pointLight[" + std::to_string(i) + "].specularStrength",
			pointLight[i].specularStrength
		);
		shader.setFloat(
			"pointLight[" + std::to_string(i) + "].constant",
			pointLight[i].constant
		);
		shader.setFloat(
			"pointLight[" + std::to_string(i) + "].linear",
			pointLight[i].linear
		);
		shader.setFloat(
			"pointLight[" + std::to_string(i) + "].quadratic",
			pointLight[i].quadratic
		);
	}
	for (int i = 0; i < 4; i++) {
		shader.setBool(
			"directionalLight[" + std::to_string(i) + "].enabled",
			directionalLight[i].enabled
		);
		glm::vec3 lightDirection(
			std::cos(directionalLight[i].lightDirection[0]) * std::cos(directionalLight[i].lightDirection[1]),
			std::sin(directionalLight[i].lightDirection[1]),
			std::sin(directionalLight[i].lightDirection[0]) * std::cos(directionalLight[i].lightDirection[1])
		);
		if (transformLight)
			shader.setVec3(
				"directionalLight[" + std::to_string(i) + "].lightDirection",
				glm::mat3(modelView) * lightDirection
			);
		else
			shader.setVec3(
				"directionalLight[" + std::to_string(i) + "].lightDirection",
				lightDirection
			);
		shader.setVec3(
			"directionalLight[" + std::to_string(i) + "].lightColor",
			directionalLight[i].lightColor
		);
		shader.setFloat(
			"directionalLight[" + std::to_string(i) + "].ambientStrength",
			directionalLight[i].ambientStrength
		);
		shader.setFloat(
			"directionalLight[" + std::to_string(i) + "].specularStrength",
			directionalLight[i].specularStrength
		);
	}
	for (int i = 0; i < 4; i++) {
		shader.setBool(
			"spotLight[" + std::to_string(i) + "].enabled",
			spotLight[i].enabled
		);
		if (transformLight)
			shader.setVec3(
				"spotLight[" + std::to_string(i) + "].lightPosition",
				modelView * glm::vec4(spotLight[i].lightPosition, 1.0f)
			);
		else
			shader.setVec3(
				"spotLight[" + std::to_string(i) + "].lightPosition",
				spotLight[i].lightPosition
			);
		glm::vec3 lightDirection(
			std::cos(spotLight[i].lightDirection[0]) * std::cos(spotLight[i].lightDirection[1]),
			std::sin(spotLight[i].lightDirection[1]),
			std::sin(spotLight[i].lightDirection[0]) * std::cos(spotLight[i].lightDirection[1])
		);
		if (transformLight)
			shader.setVec3(
				"spotLight[" + std::to_string(i) + "].lightDirection",
				glm::mat3(modelView) * lightDirection
			);
		else
			shader.setVec3(
				"spotLight[" + std::to_string(i) + "].lightDirection",
				lightDirection
			);
		shader.setFloat(
			"spotLight[" + std::to_string(i) + "].innerCosinCutOff",
			std::cos(spotLight[i].innerCutOff)
		);
		shader.setFloat(
			"spotLight[" + std::to_string(i) + "].outerCosinCutOff",
			std::cos(spotLight[i].outerCutOff)
		);
		shader.setVec3(
			"spotLight[" + std::to_string(i) + "].lightColor",
			spotLight[i].lightColor
		);
		shader.setFloat(
			"spotLight[" + std::to_string(i) + "].ambientStrength",
			spotLight[i].ambientStrength
		);
		shader.setFloat(
			"spotLight[" + std::to_string(i) + "].specularStrength",
			spotLight[i].specularStrength
		);
		shader.setFloat(
			"spotLight[" + std::to_string(i) + "].constant",
			spotLight[i].constant
		);
		shader.setFloat(
			"spotLight[" + std::to_string(i) + "].linear",
			spotLight[i].linear
		);
		shader.setFloat(
			"spotLight[" + std::to_string(i) + "].quadratic",
			spotLight[i].quadratic
		);
	}
}

jjyou::gl::Object3DViewer<0> win;
jjyou::gl::Shader shader;
jjyou::gl::PolygonMesh::Ptr mesh;
bool transformLight = false; //whether to apply `modelView` matrix transform on light sources

//shader uniform variables
bool enableLight = true;
PointLight pointLight[4] = {
	PointLight(true,  glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.7f), 0.1f, 0.1f, 1.0f, 0.0f, 0.0f),
	PointLight(false, glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.7f), 0.1f, 0.1f, 1.0f, 0.0f, 0.0f),
	PointLight(false, glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.7f), 0.1f, 0.1f, 1.0f, 0.0f, 0.0f),
	PointLight(false, glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.7f), 0.1f, 0.1f, 1.0f, 0.0f, 0.0f),
};
DirectionalLight directionalLight[4] = {
	DirectionalLight(false, glm::vec2(1.5f * std::numbers::pi_v<float>, 0.0f), glm::vec3(0.7f), 0.1f, 0.1f),
	DirectionalLight(false, glm::vec2(1.5f * std::numbers::pi_v<float>, 0.0f), glm::vec3(0.7f), 0.1f, 0.1f),
	DirectionalLight(false, glm::vec2(1.5f * std::numbers::pi_v<float>, 0.0f), glm::vec3(0.7f), 0.1f, 0.1f),
	DirectionalLight(false, glm::vec2(1.5f * std::numbers::pi_v<float>, 0.0f), glm::vec3(0.7f), 0.1f, 0.1f)
};
SpotLight spotLight[4] = {
	SpotLight(false, glm::vec3(0.0f, 0.0f, 2.0f), glm::vec2(1.5f * std::numbers::pi_v<float>, 0.0f), 12.5f / 180.0f * std::numbers::pi_v<float>, 15.0f / 180.0f * std::numbers::pi_v<float>, glm::vec3(0.7f), 0.1f, 0.1f, 1.0f, 0.0f, 0.0f),
	SpotLight(false, glm::vec3(0.0f, 0.0f, 2.0f), glm::vec2(1.5f * std::numbers::pi_v<float>, 0.0f), 12.5f / 180.0f * std::numbers::pi_v<float>, 15.0f / 180.0f * std::numbers::pi_v<float>, glm::vec3(0.7f), 0.1f, 0.1f, 1.0f, 0.0f, 0.0f),
	SpotLight(false, glm::vec3(0.0f, 0.0f, 2.0f), glm::vec2(1.5f * std::numbers::pi_v<float>, 0.0f), 12.5f / 180.0f * std::numbers::pi_v<float>, 15.0f / 180.0f * std::numbers::pi_v<float>, glm::vec3(0.7f), 0.1f, 0.1f, 1.0f, 0.0f, 0.0f),
	SpotLight(false, glm::vec3(0.0f, 0.0f, 2.0f), glm::vec2(1.5f * std::numbers::pi_v<float>, 0.0f), 12.5f / 180.0f * std::numbers::pi_v<float>, 15.0f / 180.0f * std::numbers::pi_v<float>, glm::vec3(0.7f), 0.1f, 0.1f, 1.0f, 0.0f, 0.0f),
};
float shininess = 32.0f;

//draw mode for jjyou::gl::PolygonMesh
bool drawPoints = false;
bool drawEdges = false;
bool drawFaceEdges = false;
bool drawFaces = true;


void draw(void) {
	//activate shader
	shader.use();
	
	//setup UI
	if (ImGui::Begin("Render Options")) {
		if (ImGui::TreeNode("Lighting")) {
			ImGui::Checkbox("Enable light", &enableLight);
			ImGui::Checkbox("Transform light", &transformLight);
			for (int i = 0; i < 4; i++) {
				if (ImGui::TreeNode(("Point light " + std::to_string(i)).c_str())) {
					ImGui::Checkbox("Enabled", &pointLight[i].enabled);
					ImGui::SliderFloat3("Position", &pointLight[i].lightPosition[0], -10.0f, 10.0f);
					ImGui::ColorEdit3("Color", &pointLight[i].lightColor[0]);
					ImGui::SliderFloat("Ambient", &pointLight[i].ambientStrength, 0.0f, 1.0f);
					ImGui::SliderFloat("Specular", &pointLight[i].specularStrength, 0.0f, 1.0f);
					if (ImGui::TreeNode("Attenuation")) {
						ImGui::SliderFloat("Constant", &pointLight[i].constant, 1.0f, 5.0f);
						ImGui::SliderFloat("Linear", &pointLight[i].linear, 0.0f, 1.0f);
						ImGui::SliderFloat("Quadratic", &pointLight[i].quadratic, 0.0f, 1.0f);
						ImGui::TreePop();
					}
					ImGui::TreePop();
				}
			}
			for (int i = 0; i < 4; i++) {
				if (ImGui::TreeNode(("Directional light " + std::to_string(i)).c_str())) {
					ImGui::Checkbox("Enabled", &directionalLight[i].enabled);
					ImGui::SliderAngle("Yaw", &directionalLight[i].lightDirection[0], 0.0f, 360.0f);
					ImGui::SliderAngle("Pitch", &directionalLight[i].lightDirection[1], -90.0f, 90.0f);
					ImGui::ColorEdit3("Color", &directionalLight[i].lightColor[0]);
					ImGui::SliderFloat("Ambient", &directionalLight[i].ambientStrength, 0.0f, 1.0f);
					ImGui::SliderFloat("Specular", &directionalLight[i].specularStrength, 0.0f, 1.0f);
					ImGui::TreePop();
				}
			}
			for (int i = 0; i < 4; i++) {
				if (ImGui::TreeNode(("Spotlight " + std::to_string(i)).c_str())) {
					ImGui::Checkbox("Enabled", &spotLight[i].enabled);
					ImGui::SliderFloat3("Position", &spotLight[i].lightPosition[0], -10.0f, 10.0f);
					ImGui::SliderAngle("Yaw", &spotLight[i].lightDirection[0], 0.0f, 360.0f);
					ImGui::SliderAngle("Pitch", &spotLight[i].lightDirection[1], -90.0f, 90.0f);
					ImGui::SliderAngle("Inner cutoff angle", &spotLight[i].innerCutOff, 0.0f, spotLight[i].outerCutOff / std::numbers::pi_v<float> * 180.0f);
					ImGui::SliderAngle("Outer cutoff angle", &spotLight[i].outerCutOff, spotLight[i].innerCutOff / std::numbers::pi_v<float> * 180.0f, 90.0f);
					ImGui::ColorEdit3("Color", &spotLight[i].lightColor[0]);
					ImGui::SliderFloat("Ambient", &spotLight[i].ambientStrength, 0.0f, 1.0f);
					ImGui::SliderFloat("Specular", &spotLight[i].specularStrength, 0.0f, 1.0f);
					if (ImGui::TreeNode("Attenuation")) {
						ImGui::SliderFloat("Constant", &spotLight[i].constant, 1.0f, 5.0f);
						ImGui::SliderFloat("Linear", &spotLight[i].linear, 0.0f, 1.0f);
						ImGui::SliderFloat("Quadratic", &spotLight[i].quadratic, 0.0f, 1.0f);
						ImGui::TreePop();
					}
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Object")) {
			ImGui::SliderFloat("Shininess", &shininess, 0.0f, 128.0f);
			if (ImGui::TreeNode("Render mode")) {
				ImGui::Checkbox("Points", &drawPoints);
				ImGui::Checkbox("Edges", &drawEdges);
				ImGui::Checkbox("Face edges", &drawFaceEdges);
				ImGui::Checkbox("Faces", &drawFaces);
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}
	}
	ImGui::End();

	//set shader uniforms
	shader.setMat4("modelMatrix", win.getModelMatrix());
	shader.setMat4("viewMatrix", win.getViewMatrix());
	shader.setMat4("projectionMatrix", win.getProjectionMatrix());
	shader.setMat3("normalMatrix", win.getNormalMatrix());
	shader.setVec3("viewPosition", win.getViewPosition());
	shader.setBool("enableLight", enableLight);
	shader.setFloat("shininess", shininess);
	setLightingUniformVariables(
		shader,
		transformLight,
		win.getModelMatrix(),
		pointLight,
		directionalLight,
		spotLight
	);

	//render mesh
	int drawMode = 0b0000;
	drawMode |= (drawPoints) ? jjyou::gl::PolygonMesh::DrawPoints : 0b0000;
	drawMode |= (drawEdges) ? jjyou::gl::PolygonMesh::DrawEdges : 0b0000;
	drawMode |= (drawFaceEdges) ? jjyou::gl::PolygonMesh::DrawFaceEdges : 0b0000;
	drawMode |= (drawFaces) ? jjyou::gl::PolygonMesh::DrawFaces : 0b0000;
	mesh->setDrawMode(drawMode);
	mesh->draw();

	//render UI
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

int main() {
	//create window
	win.createGLFWWindow();
	//load shader
	shader.load(jjyou::gl::ShaderCode::color_P3C4N3_multiLights_v, jjyou::gl::ShaderCode::color_P3C4N3_multiLights_f);
	//load mesh
	jjyou::io::PlyFile<double, unsigned char, false> pcd;
	pcd.read("Mesh.ply");
	//assign white color to vertices, if there is no vertex color
	pcd.vertexColor.resize(pcd.vertex.size(), decltype(pcd)::ColorType(255, 255, 255));
	//compute face and vertex normals
	pcd.vertexNormal.resize(pcd.vertex.size());
	pcd.faceNormal.resize(pcd.face.size());
	std::vector<int> numAdjacentFaces(pcd.vertex.size(), 0);
	for (int i = 0; i < pcd.face.size(); i++)
		if (pcd.face[i].size() >= 3) {
			pcd.faceNormal[i] = (pcd.vertex[pcd.face[i][1]] - pcd.vertex[pcd.face[i][0]]).cross(pcd.vertex[pcd.face[i][2]] - pcd.vertex[pcd.face[i][1]]).normalized();
			for (const auto& v : pcd.face[i]) {
				pcd.vertexNormal[v] += pcd.faceNormal[i];
				numAdjacentFaces[v]++;
			}
		}
	for (int v = 0; v < pcd.vertex.size(); v++)
		pcd.vertexNormal[v] /= numAdjacentFaces[v];
	//Instantiate jjyou::gl::PolygonMesh for OpenGL rendering
	mesh.reset(new jjyou::gl::PolygonMesh(shader, pcd));
	//rendering mainloop
	while (!glfwWindowShouldClose(win.window)) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		draw();
		glfwSwapBuffers(win.window);
		glfwPollEvents();
	}
	win.destroyGLFWWindow();
	return 0;
}
