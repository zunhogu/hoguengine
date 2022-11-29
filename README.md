# hoguengine
hoguengine은 DirectX11을 기반으로하는 3D 게임 개발을 위한 게임엔진이다.


## 프로젝트 구조

프로젝트의 구조는 다음과 같다.

![structure](https://user-images.githubusercontent.com/97880907/201290163-d17da171-8277-4f35-83d3-7dedbc3e9a33.png)

Client.exe : WinAPI 응용프로그램이 동작하는 exe

Geometry.dll : 엔진의 Core가 되는 부분으로 해당 dll에서 Scene내에 Object들이 배치되고 Render.dll과 Collision.dll과 상호작용하며 엔진의 동작을 수행하는 dll

Renderer.dll : DirectX11를 초기화 하거나, 셰이더 컴파일, 셰이더 파라미터 세팅 등 실제 Rendering을 담당하는 dll

Collision.dll : 씬 내의 오브젝트들에 대한 충돌처리를 담당하는 dll

FileSystem.dll : 로컬 영역에 존재하는 파일이나 리소스들을 Load 하여 메모리에 올리거나 Save하여 로컬파일로 내보내는 역할을 하는 dll

Utility.dll : 프로젝트에서 사용하는 수학이나, 문자열처리 등의 기능을 하는 dll

상세 링크 : <https://blog.naver.com/junhogoo/222886954474>

## 구현 목록

* FBX SDK Import
  * FBX로 추출한 데이터(Mesh, Material, Bone, Animation) 정보를 엔진에 맞게 Import 하는 기능

* UI
  * ViewPort : Scene에 대한 렌더링 결과를 출력하는 화면
    * ViewPort Picking : ViewPort에 마우스를 클릭하여 오브젝트를 피킹할 수 있는 기능 
    * ImGuizmo의 Gizmo를 이용한 Object의 Node별로 Translation, Rotation, Scale 변환  
    * Frustum Culling : Frustum Culling을 통한 렌더링 연산 최소화
  * Hierachy : Object의 Node를 트리구조로 출력, Object Delete, Copy, Paste 기능 
  * Inspector : Object의 Node가 가지고있는 Component에 대한 상세정보를 출력 및 편집
    * Model Info Component : Object의 기본정보를 담고 있는 Component
    * Transform Component : Obejct의 Transform 정보를 담고 있는 Component
    * Mesh Component : Object의 Mesh에 대한 정보를 담고 있는 Component
    * Material Component : Object의 사용하는 Material의 정보를 담고 있는 Component
    * Animation Component : Obejct가 가지고 있는 Animation을 Controll하는 Component
    * Terrain Component : Terrain Rendering을 위한 Component로 mesh에 대한 정보나 editor 기능을 수행할 수 있다. 
  * Content Browser Panel : 로컬 작업영역의 디렉토리를 순회하며 디렉토리와 파일을 조회 및 Drag and Drop 기능으로 파일 

* Animation
  * Exporter에서 추출한 Animation 정보를 가지고 Animation을 실행시킬 수 있는 기능
  
* Terrain Rendering
  * Sky Dome 구현
  * Quad Tree 구현  
  * Level of Detail 구현
  
* Manager Class로 엔진 관리
  * Manager는 엔진에서 발생하는 특정 작업을 수행하기 위한 클래스로 Singleton 패턴으로 구현
  * Scene Manager : Scene을 생성 삭제하거나 Scene 내의 Object를 추가할 수 있는 클래스
  * Resource Manager : 엔진에서 사용하는 Resource를 관리, 모든 Component는 Resource에 접근하기 위해 해당 클래스를 거처야함
  * Timer Manager : 엔진의 FPS와 CPU 사용량일 실시간으로 계산하는 클래스
  * Key Manager : 엔진의 키보드, 마우스에 대한 모든 인풋을 관리하는 클래스
  * Path Manager : 리소스를 가져오기 위한 경로를 관리하는 클래스
  * Shader Manager : 로드된 셰이더 파일들에 대해 전체적인 관리를 하는 클래스
  

## 시연 영상

오브젝트 편집
![20221016_편집](https://user-images.githubusercontent.com/97880907/196042041-8ebd2c20-6d01-43e7-8d7e-f4a8347f1fdd.gif)

애니메이션
![20221016_애니메이션](https://user-images.githubusercontent.com/97880907/196042037-8d3d6ead-131c-4832-8ec7-988205129b36.gif)

지형렌더링 Level of Detail
![녹화_2022_11_06_20_46_23_804](https://user-images.githubusercontent.com/97880907/200168685-f499ce58-e375-4e92-acd8-0c87413ea6f8.gif)

지형렌더링 Terrain Editor
![녹화_2022_11_06_20_28_03_299](https://user-images.githubusercontent.com/97880907/200168510-62839c4d-3332-4554-9f86-d3f2330061b3.gif)
![Terrain Editor](https://user-images.githubusercontent.com/97880907/200875212-ca0611c0-d1e1-4627-b03a-1cc14a0f343c.gif)


## 업데이트 내역
- 2022.09.27 : Create Repository
- 2022.10.16 : Grid 및 SkyDome 구현
- 2022.10.18 : 지형렌더링 Quad Tree 구현
- 2022.10.31 : 지형렌더링 Level of Detail 구현
- 2022.11.09 : 지형렌더링 Terrain Editor 구현

## 외부라이브러리
* ImGui
* ImGuizmo
* DirectX
* DirectXInput
* DirectXTex
* DirectX

