# Arithmetic Expression Generator

## Installation

-   Install dependencies using `npm`:

```
npm install
```

-   Install `node-gyp` using `npm`:

```
npm install -g node-gyp
```

-   Depending on your operating system, you will need to install additional tools. Please read [Installation | node-gyp](https://github.com/nodejs/node-gyp#Installation)

## Usage

-   Generate the appropriate project build files for the current platform:

```
node-gyp configure
```

-   Build:

```
node-gyp build
```

-   Build native module for `Electron`:

Please read [Using Native Node Modules | Electron](https://www.electronjs.org/docs/tutorial/using-native-node-modules)

-   Unit Test:

```
npm run test
```
