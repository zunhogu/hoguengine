# hoguengine
hoguengine은 DirectX11을 기반으로하는 3D 게임 개발을 위한 게임엔진이다.


## 프로젝트 구조

프로젝트의 구조는 다음과 같다.

![structure](https://user-images.githubusercontent.com/97880907/201290163-d17da171-8277-4f35-83d3-7dedbc3e9a33.png)

Client.exe : WinAPI 응용프로그램이 동작하는 exe

Geometry.dll : 엔진의 Core가 되는 부분으로 해당 dll에서 Scene내에 Object들이 배치되고 Render.dll과 Collision.dll과 상호작용하며 엔진의 동작을 수행하는 dll

Renderer.dll : Rendering을 담당하는 dll로 DirectX11를 초기화 하거나, 셰이더 관리, 버퍼나 텍스처를 그래픽 파이프라인에 바인딩 하는 등 렌더링에 관련된 작업을 수행하는 dll

Collision.dll : Geometry상에서 정보를 받아 Collision Test를 담당하는 dll

FileSystem.dll : 로컬 영역에 존재하는 파일을 Store / Load 등의 기능을 수행하는 dll

Utility.dll : 프로젝트에서 사용하는 수학이나, 문자열처리 등과 같은 매개변수나 함수를 모아두는 dll

상세 링크 : <https://blog.naver.com/junhogoo/222886954474>

## 구현 목록

* FBX SDK Import
  * FBX로 추출한 데이터(Mesh, Material, Bone, Animation) 정보를 엔진에 맞게 파싱

* UI
  * ViewPort : Scene을 렌더링, 현재는 Test를 위해 주변광, 난반사광, 정반사광의 효과만 적용
    * ViewPort Picking : 비용을 줄이기 위해 1차 피킹(월드공간상에서 경계입체와 반직선 충돌 체크)를 통과한 오브젝트를 2차 피킹(로컬공간에서 메시와 반직선 충돌 체크) 
    * ImGuizmo의 Gizmo를 이용한 Object의 Node별로 Translation, Rotation, Scale 변환  
    * Frustum Culling : Frustum Culling을 통한 최적화
    * Grid 및 SkyDome 구현
  * Hierachy : Object의 Node를 트리구조로 출력, Object Delete, Copy, Paste 기능 
  * Inspector : Object의 Node가 가지고있는 Component에 대한 상세정보를 출력 및 편집
    * Model Info Component : Object의 기본정보를 담고 있는 Component
    * Transform Component : Obejct의 Transform 정보를 담고 있는 Component
    * Mesh Component : Object의 Skinned Mesh에 대한 정보를 담고 있는 Component
    * Material Component : Object의 사용하는 Material의 정보를 담고 있는 Component
    * Animation Component : Obejct의 Animation을 Control하는 Component
  * Content Browser Panel : 로컬 작업영역의 디렉토리를 순회하며 디렉토리와 파일을 조회 및 Drag and Drop 기능으로 파일 

* Animation
  * FBX 기준으로 맞춰진 Animation KeyFrame에 대한 Transform 정보를 엔진에 맞는 Transform으로 쿼터니언을 이용한 구면 보간
  
* Terrain Rendering
  * Quad Tree 구현  
  * Level of Detail 구현
  
* Manager Class로 엔진 관리
  * Manager는 엔진에서 발생하는 특정 작업을 수행하기 위한 클래스로 Singleton 패턴으로 구현
  * Scene Manager : Scene을 생성 삭제하거나 Scene 안에 있는 Object 생성 삭제 하는 기능을 수행하는 Manager Class
  * Resource Manager : 엔진에서 사용하는 Resource를 관리, 모든 Resource들은 해당 Manager Class를 통해서 수행된다.
  * Timer Manager : 엔진의 FPS와 Delta Time을 실시간으로 계산
  * Key Manager : DirectX Input을 통한 키보드, 마우스 입력 감지
  * Path Manager : 작업 영역의 절대경로 계산
  * Shader Manager : Shader를 컴파일하고 렌더링 파이프라인에 제출하는 등 Shader를 관리하는 클래스
  

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

지형렌더링 쿼드 트리
![Quad Tree](https://user-images.githubusercontent.com/97880907/204195752-172c6bd3-11b7-423b-94d0-e17e90aac645.gif)



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

