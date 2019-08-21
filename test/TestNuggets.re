module TestFramework =
  Rely.Make({
    let config =
      Rely.TestFrameworkConfig.initialize({
        snapshotDir: "test/_snapshots",
        projectDir: "",
      });
  });

open TestFramework;

describe("Testing rendered media", ({test, _}) =>
  test("test_media_file must return success (0)", ({expect}) => {
    expect.int(Lib.test_media_file("/Users/manas/Downloads/video-tool/out.mp4")).
      toBe(
      0,
    );
    expect.int(
      Lib.test_media_file(
        "/home/manas/Downloads/video-tool/demo-video/output2.webm",
      ),
    ).
      toBe(
      -1,
    );
  })
);

cli();
