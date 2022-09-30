# hoguengine
프로젝트에 대한 설명

## 구현 목록

* FBX SDK Import
  * FBX로 추출한 데이터(Mesh, Material, Bone, Animation) 정보를 엔진에 맞게 파싱

* UI
  * ViewPort : Scene을 렌더링, 현재는 Test를 위해 주변광, 난반사광, 정반사광의 효과만 적용
    * ViewPort Picking : 비용을 줄이기 위해 1차 피킹(월드공간상에서 경계입체와 반직선 충돌 체크)를 통과한 오브젝트를 2차 피킹(로컬공간에서 메시와 반직선 충돌 체크) 
    * ImGuizmo의 Gizmo를 이용한 Object의 Node별로 Translation, Rotation, Scale 변환  
    * Frustum Culling : Frustum Culling을 통한 렌더링 연산 감소  
  * Hierachy : Object의 Node를 트리구조로 출력
  * Inspector : Object의 Node가 가지고있는 Component에 대한 상세정보를 출력 및 편집
    * Model Info Component : Object의 기본정보를 담고 있는 Component
    * Transform Component : Obejct의 Transform 정보를 담고 있는 Component
    * Mesh Component : Object의 Skinned Mesh에 대한 정보를 담고 있는 Component
    * Material Component : Object의 사용하는 Material의 정보를 담고 있는 Component
    * Animation Component : Obejct의 Animation을 Control하는 Component
  * Content Browser Panel : 로컬 작업영역의 디렉토리를 순회하며 디렉토리와 파일을 조회 및 Drag and Drop 기능으로 파일 

* Animation
  * FBX 기준으로 맞춰진 Animation Frame을 엔진에 맞는 Frame으로 구면 보간
  
* Manager 
  * Manager는 엔진에서 발생하는 특정 작업을 수행하기 위한 클래스로 Singleton 패턴으로 구현
  * Scene Manager : Scene과 Scene 안에 있는 Object를 관리
  * Resource Manager : 엔진에서 사용하는 Resource를 관리
  * Timer Manager : 엔진의 FPS와 Delta Time을 실시간으로 계산
  * Key Manager : DirectX Input을 통한 키보드, 마우스 입력 감지
  * Path Manager : 작업 영역의 절대경로 계산
  

## 프로젝트 구조

![structure](https://user-images.githubusercontent.com/97880907/193234230-b50a2010-93e3-43b0-9de8-04e0f6835ce0.png)


상세 링크 : <https://blog.naver.com/junhogoo/222886954474>

## 시연 영상

## 외부라이브러리
* ImGui
* ImGuizmo
* DirectX
* DirectXInput
* DirectXTex
* DirectX
