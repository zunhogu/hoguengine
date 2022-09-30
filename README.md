# hoguengine
프로젝트에 대한 설명


## 프로젝트 구조

프로젝트의 구조는 다음과 같다.

![structure](https://user-images.githubusercontent.com/97880907/193234230-b50a2010-93e3-43b0-9de8-04e0f6835ce0.png)

각각의 dll은 상위 Layer에 Implicit Linking 되어 사용된다.

Client.exe : Win32 응용프로그램의 윈도우가 생성되고 출력되고 루프되는 Layer

Geometry.dll : 엔진의 Core가 되는 부분으로 해당 dll에서 Manager Class들이 동작하면서 모든 작업을 관리하는 Layer

Renderer.dll : Rendering을 담당하는 dll로 DirectX11를 초기화 하거나 렌더 타겟 뷰 변경, 그래픽 파이프라인 바인딩 등 렌더링에 관련된 작업을 수행하는 Layer

Collision.dll : Geometry상에서 정보를 받아 Collision Test를 담당하는 Layer

FileSystem.dll : 로컬 영역에 존재하는 파일을 Store / Load 등의 기능을 수행하는 Layer


상세 링크 : <https://blog.naver.com/junhogoo/222886954474>

## 구현 목록

* FBX SDK Import
  * FBX로 추출한 데이터(Mesh, Material, Bone, Animation) 정보를 엔진에 맞게 파싱

* UI
  * ViewPort : Scene을 렌더링, 현재는 Test를 위해 주변광, 난반사광, 정반사광의 효과만 적용
    * ViewPort Picking : 비용을 줄이기 위해 1차 피킹(월드공간상에서 경계입체와 반직선 충돌 체크)를 통과한 오브젝트를 2차 피킹(로컬공간에서 메시와 반직선 충돌 체크) 
    * ImGuizmo의 Gizmo를 이용한 Object의 Node별로 Translation, Rotation, Scale 변환  
    * Frustum Culling : Frustum Culling을 통한 렌더링 연산 감소  
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
  
* Manager 
  * Manager는 엔진에서 발생하는 특정 작업을 수행하기 위한 클래스로 Singleton 패턴으로 구현
  * Scene Manager : Scene을 생성 삭제하거나 Scene 안에 있는 Object 생성 삭제 하는 기능을 수행하는 Manager Class
  * Resource Manager : 엔진에서 사용하는 Resource를 관리, 모든 Resource들은 해당 Manager Class를 통해서 수행된다.
  * Timer Manager : 엔진의 FPS와 Delta Time을 실시간으로 계산
  * Key Manager : DirectX Input을 통한 키보드, 마우스 입력 감지
  * Path Manager : 작업 영역의 절대경로 계산
  

## 시연 영상

## 외부라이브러리
* ImGui
* ImGuizmo
* DirectX
* DirectXInput
* DirectXTex
* DirectX
