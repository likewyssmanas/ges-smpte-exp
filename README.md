# nuggets

[![Build Status](https://dev.azure.com/manas0504/Nuggets/_apis/build/status/prometheansacrifice.nuggets-pesy?branchName=master)](https://dev.azure.com/manas0504/Nuggets/_build/latest?definitionId=1&branchName=master)


**Contains the following libraries and executables:**

```
nuggets@0.0.0
│
├─test/
│   name:    TestNuggets.exe
│   require: nuggets/library
│
├─library/
│   library name: nuggets/library
│   require:
│
└─executable/
    name:    NuggetsApp.exe
    require: nuggets/library
```

## Developing:

```
npm install -g esy
git clone <this-repo>
esy install
esy build
```

## Running Binary:

After building the project, you can run the main binary that is produced.

```
esy x NuggetsApp.exe 
```

## Running Tests:

```
# Runs the "test" command in `package.json`.
esy test
```
