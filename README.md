# B-Tree 구현 

## 1. 요구 기능
* Insertion & Search
    * input.csv에 있는 값들을 모두 읽어 삽입 연산 수행
    * Tree 내에 있는 100만개의 key들을 모두 search하여 그 value를 새로운 파일에 입력한 뒤, 새 파일을 원래의 input.csv 파일과 비교

* Deletion
    * Tree 내에 있는 input.csv의 전체 100만개 key-value쌍 중 절반에 대하여 삭제 연산 수행
        * 삭제 대상인 key-value 쌍은 delete.csv에 제시되어 있음
    * Tree 내에 남아있는 key들을 모두 search하여 그 value를 새로운 파일에 입력한 뒤, 새 파일을 delete_compare.csv 파일과 비교
        * 기존의 데이터셋에서 삭제된 value들은 생성된 새 파일 내에 "없는 값"(e.g. N/A, NF)이라고 출력되어야 함


## 2. 테스트 방법
  1. 프로그램을 실행하면 어느 연산을 수행할 것인지 선택(ex: 1.insertion 2.deletion 3.quit)
  2. 숫자 1을 입력하면 insert할 파일 이름을 입력하는 화면이 나타나고, 삽입할 정보가 담긴 파일 이름 입력
  3. 파일 이름을 입력하면 insertion, search, compare를 자동으로 수행하고 결과 화면 출력
  4. 3의 동작 종료 후 1의 화면이 다시 나타나고 숫자 2를 입력하면 delete할 파일 이름을 입력하는 화면이 나타나고, 삭제 정보가 담긴 파일 이름 입력
  5. 파일 이름을 입력하면 deletion, search, compare를 자동으로 수행하고 결과 화면 출력
  6. 5의 동작 종료 후 1의 화면이 다시 나타나고, 3을 입력하여 프로그램 종료


## 3. 목표 구성
1. 소스코드
2. 실행파일
3. 입/출력 파일
4. 테스트 절차서
5. 간단한 명세서(program description)
6. 개발 및 실행 환경 상세(OS, 언어 버전, IDE 종류 및 버전 등)


## 4. 목표 기한
* 삽입&검색: ~9.1.(금)
* 삭제: 9.2.(토) ~ 9.8.(금)

