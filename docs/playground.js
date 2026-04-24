// easyc.ts
var fs = (() => ({}));

// node:path
function assertPath(path) {
  if (typeof path !== "string")
    throw TypeError("Path must be a string. Received " + JSON.stringify(path));
}
function normalizeStringPosix(path, allowAboveRoot) {
  var res = "", lastSegmentLength = 0, lastSlash = -1, dots = 0, code;
  for (var i = 0;i <= path.length; ++i) {
    if (i < path.length)
      code = path.charCodeAt(i);
    else if (code === 47)
      break;
    else
      code = 47;
    if (code === 47) {
      if (lastSlash === i - 1 || dots === 1)
        ;
      else if (lastSlash !== i - 1 && dots === 2) {
        if (res.length < 2 || lastSegmentLength !== 2 || res.charCodeAt(res.length - 1) !== 46 || res.charCodeAt(res.length - 2) !== 46) {
          if (res.length > 2) {
            var lastSlashIndex = res.lastIndexOf("/");
            if (lastSlashIndex !== res.length - 1) {
              if (lastSlashIndex === -1)
                res = "", lastSegmentLength = 0;
              else
                res = res.slice(0, lastSlashIndex), lastSegmentLength = res.length - 1 - res.lastIndexOf("/");
              lastSlash = i, dots = 0;
              continue;
            }
          } else if (res.length === 2 || res.length === 1) {
            res = "", lastSegmentLength = 0, lastSlash = i, dots = 0;
            continue;
          }
        }
        if (allowAboveRoot) {
          if (res.length > 0)
            res += "/..";
          else
            res = "..";
          lastSegmentLength = 2;
        }
      } else {
        if (res.length > 0)
          res += "/" + path.slice(lastSlash + 1, i);
        else
          res = path.slice(lastSlash + 1, i);
        lastSegmentLength = i - lastSlash - 1;
      }
      lastSlash = i, dots = 0;
    } else if (code === 46 && dots !== -1)
      ++dots;
    else
      dots = -1;
  }
  return res;
}
function _format(sep, pathObject) {
  var dir = pathObject.dir || pathObject.root, base = pathObject.base || (pathObject.name || "") + (pathObject.ext || "");
  if (!dir)
    return base;
  if (dir === pathObject.root)
    return dir + base;
  return dir + sep + base;
}
function resolve() {
  var resolvedPath = "", resolvedAbsolute = false, cwd;
  for (var i = arguments.length - 1;i >= -1 && !resolvedAbsolute; i--) {
    var path;
    if (i >= 0)
      path = arguments[i];
    else {
      if (cwd === undefined)
        cwd = process.cwd();
      path = cwd;
    }
    if (assertPath(path), path.length === 0)
      continue;
    resolvedPath = path + "/" + resolvedPath, resolvedAbsolute = path.charCodeAt(0) === 47;
  }
  if (resolvedPath = normalizeStringPosix(resolvedPath, !resolvedAbsolute), resolvedAbsolute)
    if (resolvedPath.length > 0)
      return "/" + resolvedPath;
    else
      return "/";
  else if (resolvedPath.length > 0)
    return resolvedPath;
  else
    return ".";
}
function normalize(path) {
  if (assertPath(path), path.length === 0)
    return ".";
  var isAbsolute = path.charCodeAt(0) === 47, trailingSeparator = path.charCodeAt(path.length - 1) === 47;
  if (path = normalizeStringPosix(path, !isAbsolute), path.length === 0 && !isAbsolute)
    path = ".";
  if (path.length > 0 && trailingSeparator)
    path += "/";
  if (isAbsolute)
    return "/" + path;
  return path;
}
function isAbsolute(path) {
  return assertPath(path), path.length > 0 && path.charCodeAt(0) === 47;
}
function join() {
  if (arguments.length === 0)
    return ".";
  var joined;
  for (var i = 0;i < arguments.length; ++i) {
    var arg = arguments[i];
    if (assertPath(arg), arg.length > 0)
      if (joined === undefined)
        joined = arg;
      else
        joined += "/" + arg;
  }
  if (joined === undefined)
    return ".";
  return normalize(joined);
}
function relative(from, to) {
  if (assertPath(from), assertPath(to), from === to)
    return "";
  if (from = resolve(from), to = resolve(to), from === to)
    return "";
  var fromStart = 1;
  for (;fromStart < from.length; ++fromStart)
    if (from.charCodeAt(fromStart) !== 47)
      break;
  var fromEnd = from.length, fromLen = fromEnd - fromStart, toStart = 1;
  for (;toStart < to.length; ++toStart)
    if (to.charCodeAt(toStart) !== 47)
      break;
  var toEnd = to.length, toLen = toEnd - toStart, length = fromLen < toLen ? fromLen : toLen, lastCommonSep = -1, i = 0;
  for (;i <= length; ++i) {
    if (i === length) {
      if (toLen > length) {
        if (to.charCodeAt(toStart + i) === 47)
          return to.slice(toStart + i + 1);
        else if (i === 0)
          return to.slice(toStart + i);
      } else if (fromLen > length) {
        if (from.charCodeAt(fromStart + i) === 47)
          lastCommonSep = i;
        else if (i === 0)
          lastCommonSep = 0;
      }
      break;
    }
    var fromCode = from.charCodeAt(fromStart + i), toCode = to.charCodeAt(toStart + i);
    if (fromCode !== toCode)
      break;
    else if (fromCode === 47)
      lastCommonSep = i;
  }
  var out = "";
  for (i = fromStart + lastCommonSep + 1;i <= fromEnd; ++i)
    if (i === fromEnd || from.charCodeAt(i) === 47)
      if (out.length === 0)
        out += "..";
      else
        out += "/..";
  if (out.length > 0)
    return out + to.slice(toStart + lastCommonSep);
  else {
    if (toStart += lastCommonSep, to.charCodeAt(toStart) === 47)
      ++toStart;
    return to.slice(toStart);
  }
}
function _makeLong(path) {
  return path;
}
function dirname(path) {
  if (assertPath(path), path.length === 0)
    return ".";
  var code = path.charCodeAt(0), hasRoot = code === 47, end = -1, matchedSlash = true;
  for (var i = path.length - 1;i >= 1; --i)
    if (code = path.charCodeAt(i), code === 47) {
      if (!matchedSlash) {
        end = i;
        break;
      }
    } else
      matchedSlash = false;
  if (end === -1)
    return hasRoot ? "/" : ".";
  if (hasRoot && end === 1)
    return "//";
  return path.slice(0, end);
}
function basename(path, ext) {
  if (ext !== undefined && typeof ext !== "string")
    throw TypeError('"ext" argument must be a string');
  assertPath(path);
  var start = 0, end = -1, matchedSlash = true, i;
  if (ext !== undefined && ext.length > 0 && ext.length <= path.length) {
    if (ext.length === path.length && ext === path)
      return "";
    var extIdx = ext.length - 1, firstNonSlashEnd = -1;
    for (i = path.length - 1;i >= 0; --i) {
      var code = path.charCodeAt(i);
      if (code === 47) {
        if (!matchedSlash) {
          start = i + 1;
          break;
        }
      } else {
        if (firstNonSlashEnd === -1)
          matchedSlash = false, firstNonSlashEnd = i + 1;
        if (extIdx >= 0)
          if (code === ext.charCodeAt(extIdx)) {
            if (--extIdx === -1)
              end = i;
          } else
            extIdx = -1, end = firstNonSlashEnd;
      }
    }
    if (start === end)
      end = firstNonSlashEnd;
    else if (end === -1)
      end = path.length;
    return path.slice(start, end);
  } else {
    for (i = path.length - 1;i >= 0; --i)
      if (path.charCodeAt(i) === 47) {
        if (!matchedSlash) {
          start = i + 1;
          break;
        }
      } else if (end === -1)
        matchedSlash = false, end = i + 1;
    if (end === -1)
      return "";
    return path.slice(start, end);
  }
}
function extname(path) {
  assertPath(path);
  var startDot = -1, startPart = 0, end = -1, matchedSlash = true, preDotState = 0;
  for (var i = path.length - 1;i >= 0; --i) {
    var code = path.charCodeAt(i);
    if (code === 47) {
      if (!matchedSlash) {
        startPart = i + 1;
        break;
      }
      continue;
    }
    if (end === -1)
      matchedSlash = false, end = i + 1;
    if (code === 46) {
      if (startDot === -1)
        startDot = i;
      else if (preDotState !== 1)
        preDotState = 1;
    } else if (startDot !== -1)
      preDotState = -1;
  }
  if (startDot === -1 || end === -1 || preDotState === 0 || preDotState === 1 && startDot === end - 1 && startDot === startPart + 1)
    return "";
  return path.slice(startDot, end);
}
function format(pathObject) {
  if (pathObject === null || typeof pathObject !== "object")
    throw TypeError('The "pathObject" argument must be of type Object. Received type ' + typeof pathObject);
  return _format("/", pathObject);
}
function parse(path) {
  assertPath(path);
  var ret = { root: "", dir: "", base: "", ext: "", name: "" };
  if (path.length === 0)
    return ret;
  var code = path.charCodeAt(0), isAbsolute2 = code === 47, start;
  if (isAbsolute2)
    ret.root = "/", start = 1;
  else
    start = 0;
  var startDot = -1, startPart = 0, end = -1, matchedSlash = true, i = path.length - 1, preDotState = 0;
  for (;i >= start; --i) {
    if (code = path.charCodeAt(i), code === 47) {
      if (!matchedSlash) {
        startPart = i + 1;
        break;
      }
      continue;
    }
    if (end === -1)
      matchedSlash = false, end = i + 1;
    if (code === 46) {
      if (startDot === -1)
        startDot = i;
      else if (preDotState !== 1)
        preDotState = 1;
    } else if (startDot !== -1)
      preDotState = -1;
  }
  if (startDot === -1 || end === -1 || preDotState === 0 || preDotState === 1 && startDot === end - 1 && startDot === startPart + 1) {
    if (end !== -1)
      if (startPart === 0 && isAbsolute2)
        ret.base = ret.name = path.slice(1, end);
      else
        ret.base = ret.name = path.slice(startPart, end);
  } else {
    if (startPart === 0 && isAbsolute2)
      ret.name = path.slice(1, startDot), ret.base = path.slice(1, end);
    else
      ret.name = path.slice(startPart, startDot), ret.base = path.slice(startPart, end);
    ret.ext = path.slice(startDot, end);
  }
  if (startPart > 0)
    ret.dir = path.slice(0, startPart - 1);
  else if (isAbsolute2)
    ret.dir = "/";
  return ret;
}
var sep = "/";
var delimiter = ":";
var posix = ((p) => (p.posix = p, p))({ resolve, normalize, isAbsolute, join, relative, _makeLong, dirname, basename, extname, format, parse, sep, delimiter, win32: null, posix: null });

// node:url
var { URL: URL2, URLSearchParams } = globalThis;
function util_isString(arg) {
  return typeof arg === "string";
}
function util_isObject(arg) {
  return typeof arg === "object" && arg !== null;
}
function util_isNull(arg) {
  return arg === null;
}
function util_isNullOrUndefined(arg) {
  return arg == null;
}
function Url() {
  this.protocol = null, this.slashes = null, this.auth = null, this.host = null, this.port = null, this.hostname = null, this.hash = null, this.search = null, this.query = null, this.pathname = null, this.path = null, this.href = null;
}
var protocolPattern = /^([a-z0-9.+-]+:)/i;
var portPattern = /:[0-9]*$/;
var simplePathPattern = /^(\/\/?(?!\/)[^\?\s]*)(\?[^\s]*)?$/;
var delims = ["<", ">", '"', "`", " ", "\r", `
`, "\t"];
var unwise = ["{", "}", "|", "\\", "^", "`"].concat(delims);
var autoEscape = ["'"].concat(unwise);
var nonHostChars = ["%", "/", "?", ";", "#"].concat(autoEscape);
var hostEndingChars = ["/", "?", "#"];
var hostnameMaxLen = 255;
var hostnamePartPattern = /^[+a-z0-9A-Z_-]{0,63}$/;
var hostnamePartStart = /^([+a-z0-9A-Z_-]{0,63})(.*)$/;
var unsafeProtocol = { javascript: true, "javascript:": true };
var hostlessProtocol = { javascript: true, "javascript:": true };
var slashedProtocol = { http: true, https: true, ftp: true, gopher: true, file: true, "http:": true, "https:": true, "ftp:": true, "gopher:": true, "file:": true };
var querystring = { parse(str) {
  var decode = decodeURIComponent;
  return (str + "").replace(/\+/g, " ").split("&").filter(Boolean).reduce(function(obj, item, index) {
    var ref = item.split("="), key = decode(ref[0] || ""), val = decode(ref[1] || ""), prev = obj[key];
    return obj[key] = prev === undefined ? val : [].concat(prev, val), obj;
  }, {});
}, stringify(obj) {
  var encode = encodeURIComponent;
  return Object.keys(obj || {}).reduce(function(arr, key) {
    return [].concat(obj[key]).forEach(function(v) {
      arr.push(encode(key) + "=" + encode(v));
    }), arr;
  }, []).join("&").replace(/\s/g, "+");
} };
function urlParse(url, parseQueryString, slashesDenoteHost) {
  if (url && util_isObject(url) && url instanceof Url)
    return url;
  var u = new Url;
  return u.parse(url, parseQueryString, slashesDenoteHost), u;
}
Url.prototype.parse = function(url, parseQueryString, slashesDenoteHost) {
  if (!util_isString(url))
    throw TypeError("Parameter 'url' must be a string, not " + typeof url);
  var queryIndex = url.indexOf("?"), splitter = queryIndex !== -1 && queryIndex < url.indexOf("#") ? "?" : "#", uSplit = url.split(splitter), slashRegex = /\\/g;
  uSplit[0] = uSplit[0].replace(slashRegex, "/"), url = uSplit.join(splitter);
  var rest = url;
  if (rest = rest.trim(), !slashesDenoteHost && url.split("#").length === 1) {
    var simplePath = simplePathPattern.exec(rest);
    if (simplePath) {
      if (this.path = rest, this.href = rest, this.pathname = simplePath[1], simplePath[2])
        if (this.search = simplePath[2], parseQueryString)
          this.query = querystring.parse(this.search.substr(1));
        else
          this.query = this.search.substr(1);
      else if (parseQueryString)
        this.search = "", this.query = {};
      return this;
    }
  }
  var proto = protocolPattern.exec(rest);
  if (proto) {
    proto = proto[0];
    var lowerProto = proto.toLowerCase();
    this.protocol = lowerProto, rest = rest.substr(proto.length);
  }
  if (slashesDenoteHost || proto || rest.match(/^\/\/[^@\/]+@[^@\/]+/)) {
    var slashes = rest.substr(0, 2) === "//";
    if (slashes && !(proto && hostlessProtocol[proto]))
      rest = rest.substr(2), this.slashes = true;
  }
  if (!hostlessProtocol[proto] && (slashes || proto && !slashedProtocol[proto])) {
    var hostEnd = -1;
    for (var i = 0;i < hostEndingChars.length; i++) {
      var hec = rest.indexOf(hostEndingChars[i]);
      if (hec !== -1 && (hostEnd === -1 || hec < hostEnd))
        hostEnd = hec;
    }
    var auth, atSign;
    if (hostEnd === -1)
      atSign = rest.lastIndexOf("@");
    else
      atSign = rest.lastIndexOf("@", hostEnd);
    if (atSign !== -1)
      auth = rest.slice(0, atSign), rest = rest.slice(atSign + 1), this.auth = decodeURIComponent(auth);
    hostEnd = -1;
    for (var i = 0;i < nonHostChars.length; i++) {
      var hec = rest.indexOf(nonHostChars[i]);
      if (hec !== -1 && (hostEnd === -1 || hec < hostEnd))
        hostEnd = hec;
    }
    if (hostEnd === -1)
      hostEnd = rest.length;
    this.host = rest.slice(0, hostEnd), rest = rest.slice(hostEnd), this.parseHost(), this.hostname = this.hostname || "";
    var ipv6Hostname = this.hostname[0] === "[" && this.hostname[this.hostname.length - 1] === "]";
    if (!ipv6Hostname) {
      var hostparts = this.hostname.split(/\./);
      for (var i = 0, l = hostparts.length;i < l; i++) {
        var part = hostparts[i];
        if (!part)
          continue;
        if (!part.match(hostnamePartPattern)) {
          var newpart = "";
          for (var j = 0, k = part.length;j < k; j++)
            if (part.charCodeAt(j) > 127)
              newpart += "x";
            else
              newpart += part[j];
          if (!newpart.match(hostnamePartPattern)) {
            var validParts = hostparts.slice(0, i), notHost = hostparts.slice(i + 1), bit = part.match(hostnamePartStart);
            if (bit)
              validParts.push(bit[1]), notHost.unshift(bit[2]);
            if (notHost.length)
              rest = "/" + notHost.join(".") + rest;
            this.hostname = validParts.join(".");
            break;
          }
        }
      }
    }
    if (this.hostname.length > hostnameMaxLen)
      this.hostname = "";
    else
      this.hostname = this.hostname.toLowerCase();
    if (!ipv6Hostname)
      this.hostname = new URL2(`https://${this.hostname}`).hostname;
    var p = this.port ? ":" + this.port : "", h = this.hostname || "";
    if (this.host = h + p, this.href += this.host, ipv6Hostname) {
      if (this.hostname = this.hostname.substr(1, this.hostname.length - 2), rest[0] !== "/")
        rest = "/" + rest;
    }
  }
  if (!unsafeProtocol[lowerProto])
    for (var i = 0, l = autoEscape.length;i < l; i++) {
      var ae = autoEscape[i];
      if (rest.indexOf(ae) === -1)
        continue;
      var esc = encodeURIComponent(ae);
      if (esc === ae)
        esc = escape(ae);
      rest = rest.split(ae).join(esc);
    }
  var hash = rest.indexOf("#");
  if (hash !== -1)
    this.hash = rest.substr(hash), rest = rest.slice(0, hash);
  var qm = rest.indexOf("?");
  if (qm !== -1) {
    if (this.search = rest.substr(qm), this.query = rest.substr(qm + 1), parseQueryString)
      this.query = querystring.parse(this.query);
    rest = rest.slice(0, qm);
  } else if (parseQueryString)
    this.search = "", this.query = {};
  if (rest)
    this.pathname = rest;
  if (slashedProtocol[lowerProto] && this.hostname && !this.pathname)
    this.pathname = "/";
  if (this.pathname || this.search) {
    var p = this.pathname || "", s = this.search || "";
    this.path = p + s;
  }
  return this.href = this.format(), this;
};
Url.prototype.format = function() {
  var auth = this.auth || "";
  if (auth)
    auth = encodeURIComponent(auth), auth = auth.replace(/%3A/i, ":"), auth += "@";
  var protocol = this.protocol || "", pathname = this.pathname || "", hash = this.hash || "", host = false, query = "";
  if (this.host)
    host = auth + this.host;
  else if (this.hostname) {
    if (host = auth + (this.hostname.indexOf(":") === -1 ? this.hostname : "[" + this.hostname + "]"), this.port)
      host += ":" + this.port;
  }
  if (this.query && util_isObject(this.query) && Object.keys(this.query).length)
    query = querystring.stringify(this.query);
  var search = this.search || query && "?" + query || "";
  if (protocol && protocol.substr(-1) !== ":")
    protocol += ":";
  if (this.slashes || (!protocol || slashedProtocol[protocol]) && host !== false) {
    if (host = "//" + (host || ""), pathname && pathname.charAt(0) !== "/")
      pathname = "/" + pathname;
  } else if (!host)
    host = "";
  if (hash && hash.charAt(0) !== "#")
    hash = "#" + hash;
  if (search && search.charAt(0) !== "?")
    search = "?" + search;
  return pathname = pathname.replace(/[?#]/g, function(match) {
    return encodeURIComponent(match);
  }), search = search.replace("#", "%23"), protocol + host + pathname + search + hash;
};
Url.prototype.resolve = function(relative2) {
  return this.resolveObject(urlParse(relative2, false, true)).format();
};
Url.prototype.resolveObject = function(relative2) {
  if (util_isString(relative2)) {
    var rel = new Url;
    rel.parse(relative2, false, true), relative2 = rel;
  }
  var result = new Url, tkeys = Object.keys(this);
  for (var tk = 0;tk < tkeys.length; tk++) {
    var tkey = tkeys[tk];
    result[tkey] = this[tkey];
  }
  if (result.hash = relative2.hash, relative2.href === "")
    return result.href = result.format(), result;
  if (relative2.slashes && !relative2.protocol) {
    var rkeys = Object.keys(relative2);
    for (var rk = 0;rk < rkeys.length; rk++) {
      var rkey = rkeys[rk];
      if (rkey !== "protocol")
        result[rkey] = relative2[rkey];
    }
    if (slashedProtocol[result.protocol] && result.hostname && !result.pathname)
      result.path = result.pathname = "/";
    return result.href = result.format(), result;
  }
  if (relative2.protocol && relative2.protocol !== result.protocol) {
    if (!slashedProtocol[relative2.protocol]) {
      var keys = Object.keys(relative2);
      for (var v = 0;v < keys.length; v++) {
        var k = keys[v];
        result[k] = relative2[k];
      }
      return result.href = result.format(), result;
    }
    if (result.protocol = relative2.protocol, !relative2.host && !hostlessProtocol[relative2.protocol]) {
      var relPath = (relative2.pathname || "").split("/");
      while (relPath.length && !(relative2.host = relPath.shift()))
        ;
      if (!relative2.host)
        relative2.host = "";
      if (!relative2.hostname)
        relative2.hostname = "";
      if (relPath[0] !== "")
        relPath.unshift("");
      if (relPath.length < 2)
        relPath.unshift("");
      result.pathname = relPath.join("/");
    } else
      result.pathname = relative2.pathname;
    if (result.search = relative2.search, result.query = relative2.query, result.host = relative2.host || "", result.auth = relative2.auth, result.hostname = relative2.hostname || relative2.host, result.port = relative2.port, result.pathname || result.search) {
      var p = result.pathname || "", s = result.search || "";
      result.path = p + s;
    }
    return result.slashes = result.slashes || relative2.slashes, result.href = result.format(), result;
  }
  var isSourceAbs = result.pathname && result.pathname.charAt(0) === "/", isRelAbs = relative2.host || relative2.pathname && relative2.pathname.charAt(0) === "/", mustEndAbs = isRelAbs || isSourceAbs || result.host && relative2.pathname, removeAllDots = mustEndAbs, srcPath = result.pathname && result.pathname.split("/") || [], relPath = relative2.pathname && relative2.pathname.split("/") || [], psychotic = result.protocol && !slashedProtocol[result.protocol];
  if (psychotic) {
    if (result.hostname = "", result.port = null, result.host)
      if (srcPath[0] === "")
        srcPath[0] = result.host;
      else
        srcPath.unshift(result.host);
    if (result.host = "", relative2.protocol) {
      if (relative2.hostname = null, relative2.port = null, relative2.host)
        if (relPath[0] === "")
          relPath[0] = relative2.host;
        else
          relPath.unshift(relative2.host);
      relative2.host = null;
    }
    mustEndAbs = mustEndAbs && (relPath[0] === "" || srcPath[0] === "");
  }
  if (isRelAbs)
    result.host = relative2.host || relative2.host === "" ? relative2.host : result.host, result.hostname = relative2.hostname || relative2.hostname === "" ? relative2.hostname : result.hostname, result.search = relative2.search, result.query = relative2.query, srcPath = relPath;
  else if (relPath.length) {
    if (!srcPath)
      srcPath = [];
    srcPath.pop(), srcPath = srcPath.concat(relPath), result.search = relative2.search, result.query = relative2.query;
  } else if (!util_isNullOrUndefined(relative2.search)) {
    if (psychotic) {
      result.hostname = result.host = srcPath.shift();
      var authInHost = result.host && result.host.indexOf("@") > 0 ? result.host.split("@") : false;
      if (authInHost)
        result.auth = authInHost.shift(), result.host = result.hostname = authInHost.shift();
    }
    if (result.search = relative2.search, result.query = relative2.query, !util_isNull(result.pathname) || !util_isNull(result.search))
      result.path = (result.pathname ? result.pathname : "") + (result.search ? result.search : "");
    return result.href = result.format(), result;
  }
  if (!srcPath.length) {
    if (result.pathname = null, result.search)
      result.path = "/" + result.search;
    else
      result.path = null;
    return result.href = result.format(), result;
  }
  var last = srcPath.slice(-1)[0], hasTrailingSlash = (result.host || relative2.host || srcPath.length > 1) && (last === "." || last === "..") || last === "", up = 0;
  for (var i = srcPath.length;i >= 0; i--)
    if (last = srcPath[i], last === ".")
      srcPath.splice(i, 1);
    else if (last === "..")
      srcPath.splice(i, 1), up++;
    else if (up)
      srcPath.splice(i, 1), up--;
  if (!mustEndAbs && !removeAllDots)
    for (;up--; up)
      srcPath.unshift("..");
  if (mustEndAbs && srcPath[0] !== "" && (!srcPath[0] || srcPath[0].charAt(0) !== "/"))
    srcPath.unshift("");
  if (hasTrailingSlash && srcPath.join("/").substr(-1) !== "/")
    srcPath.push("");
  var isAbsolute2 = srcPath[0] === "" || srcPath[0] && srcPath[0].charAt(0) === "/";
  if (psychotic) {
    result.hostname = result.host = isAbsolute2 ? "" : srcPath.length ? srcPath.shift() : "";
    var authInHost = result.host && result.host.indexOf("@") > 0 ? result.host.split("@") : false;
    if (authInHost)
      result.auth = authInHost.shift(), result.host = result.hostname = authInHost.shift();
  }
  if (mustEndAbs = mustEndAbs || result.host && srcPath.length, mustEndAbs && !isAbsolute2)
    srcPath.unshift("");
  if (!srcPath.length)
    result.pathname = null, result.path = null;
  else
    result.pathname = srcPath.join("/");
  if (!util_isNull(result.pathname) || !util_isNull(result.search))
    result.path = (result.pathname ? result.pathname : "") + (result.search ? result.search : "");
  return result.auth = relative2.auth || result.auth, result.slashes = result.slashes || relative2.slashes, result.href = result.format(), result;
};
Url.prototype.parseHost = function() {
  var host = this.host, port = portPattern.exec(host);
  if (port) {
    if (port = port[0], port !== ":")
      this.port = port.substr(1);
    host = host.substr(0, host.length - port.length);
  }
  if (host)
    this.hostname = host;
};

// easyc.ts
var { default: child_process} = (() => ({}));
function indent(str, n = 1) {
  return str.split(/\r?\n/).map((line) => "    ".repeat(n) + line).join(`
`);
}
function emit(lines) {
  return lines.filter((line) => line.trim() !== "").join(`
`);
}
class InputText {
  filename;
  text;
  constructor(options) {
    const { text = "", filename = null } = options;
    if (text) {
      this.text = text;
      this.filename = filename ?? "";
    } else if (filename) {
      this.text = fs.readFileSync(filename, "utf-8");
      this.filename = filename;
    } else {
      this.text = "";
      this.filename = "";
    }
  }
}

class CompilerError extends Error {
}

class LexerError extends CompilerError {
}

class ParseError extends CompilerError {
  token;
  constructor(message, token) {
    super(message);
    this.token = token;
  }
  toString() {
    const token = this.token;
    const lines = token.context.text.text.split(/\r?\n/);
    const errorLine = lines[token.line - 1] || "";
    return `${this.message}
` + `at ${token.context.text.filename}:${token.line}:${token.character}
` + `${errorLine}
` + `${" ".repeat(Math.max(0, token.character - 1))}^`;
  }
}
var KEYWORDS = new Set([
  "PROGRAM",
  "BEGIN",
  "END",
  "TYPE",
  "IS",
  "NAME",
  "EXTERNAL",
  "FUNCTION",
  "PROCEDURE",
  "DECLARE",
  "SET",
  "CALL",
  "RETURN",
  "EXIT",
  "INPUT",
  "OUTPUT",
  "INTEGER",
  "REAL",
  "BOOLEAN",
  "STRING",
  "IF",
  "THEN",
  "ELSE",
  "FI",
  "FOR",
  "WHILE",
  "BY",
  "DO",
  "SELECT",
  "CASE",
  "OTHERWISE",
  "TRUE",
  "FALSE",
  "ARRAY",
  "OF",
  "STRUCTURE",
  "FIELD",
  "NOT",
  "MOD",
  "XOR"
]);
var SYMBOLS = new Set("+ - * / | & ( ) [ ] ; , . : := = <> < <= > >= ||".split(" ").filter(Boolean));

class Context {
  flags;
  text;
  tokens;
  common;
  types;
  functions;
  procedures;
  variables;
  r;
  R = () => `$r${this.r++}`;
  s;
  S = () => `$${this.s++}`;
  frames = [];
  enter_frame = () => {
    this.frames.push({ defer: [] });
  };
  leave_frame = (code) => {
    const frame = this.frames.pop();
    if (frame && frame.defer.length)
      code.push(...frame.defer.reverse().map((v) => v.code));
  };
  defer = (v) => {
    const frame = this.frames.at(-1);
    if (!frame)
      throw new GenerateError("no active frame for defer");
    const code = v.code.trim();
    const defer = frame.defer.filter((x) => x.id !== v.id);
    defer.push({ id: v.id, code: indent(code, 1) });
    frame.defer = defer;
  };
  constructor(init) {
    this.flags = init.flags ?? {};
    this.text = init.text;
    this.tokens = init.tokens ?? [];
    this.common = init.common ?? [];
    this.types = init.types;
    this.functions = init.functions ?? {};
    this.procedures = init.procedures ?? {};
    this.variables = init.variables ?? {};
    this.r = 1;
    this.s = 1;
  }
}

class Token {
  type;
  value;
  line;
  character;
  context;
  constructor(type, value, line, character, context) {
    this.type = type;
    this.value = value;
    this.line = line;
    this.character = character;
    this.context = context;
  }
  toString() {
    const v = this.type === "STRING" || this.type === "SYMBOL" ? `'${this.value.length < 20 ? this.value : this.value.slice(0, 17) + "..."}'` : this.value;
    const input = this.context.text;
    const typePart = this.type !== this.value ? `|${this.type}` : "";
    return `<${v}${typePart}|${input.filename}:${this.line}:${this.character}>`;
  }
}
function printer(root) {
  const seen = new Set;
  function walker(obj) {
    if (obj == null)
      return "";
    if (typeof obj === "boolean" || typeof obj === "number" || typeof obj === "string")
      return obj;
    if (typeof obj === "object") {
      if (seen.has(obj))
        throw new Error("cycle detected in AST: serialization would recurse forever");
      seen.add(obj);
      if (obj instanceof Token) {
        const token = obj;
        const data3 = `<${token.value}|${token.type} ${token.context.text.filename}:${token.line}:${token.character}>`;
        seen.delete(obj);
        return data3;
      }
      if (Array.isArray(obj)) {
        const sequence = obj.map((x) => walker(x));
        seen.delete(obj);
        return sequence;
      }
      const data2 = { node: obj.constructor?.name ?? "[node]" };
      for (const [name, value] of Object.entries(obj)) {
        if (name.startsWith("_"))
          continue;
        if (typeof value === "function")
          continue;
        data2[name] = walker(value);
      }
      seen.delete(obj);
      return data2;
    }
    return String(obj);
  }
  const data = walker(root);
  return JSON.stringify(data, null, 2);
}

class Type {
  c() {
    throw new GenerateError(`c() not implemented for ${this.constructor.name}`);
  }
  zero(code) {
    throw new GenerateError(`zero() not implemented for ${this.constructor.name}`);
  }
  typedef(alias) {
    throw new GenerateError(`typedef() not implemented for ${this.constructor.name}`);
  }
  format = () => "";
}

class BuiltinType extends Type {
}

class UnknownType extends Type {
  c = () => "UNKNOWN";
  zero = () => "0";
  typedef = (alias) => `typedef UNKNOWN ${alias}`;
  format = () => "?";
}

class IntegerType extends BuiltinType {
  c = () => "int";
  zero = () => "0";
  typedef = (alias) => `typedef int ${alias}`;
  format = () => "i";
}

class RealType extends BuiltinType {
  c = () => "double";
  zero = () => "0.0";
  typedef = (alias) => `typedef double ${alias}`;
  format = () => "r";
}

class BooleanType extends BuiltinType {
  c = () => "int";
  zero = () => "0";
  typedef = (alias) => `typedef int ${alias}`;
  format = () => "b";
}

class StringType extends BuiltinType {
  initial;
  constructor(initial) {
    super();
    this.initial = initial;
  }
  zero = () => this.initial ? `{ .data = "${this.initial}", .sz = ${this.initial.length} }` : "{0}";
  c = () => "STR";
  typedef = (alias) => `typedef STR ${alias}`;
  format = () => "A";
}

class ArrayType extends Type {
  type;
  hi;
  lo;
  dynamic;
  constructor(type, hi, lo, dynamic = false) {
    super();
    this.type = type;
    this.hi = hi;
    this.lo = lo;
    this.dynamic = dynamic;
  }
  c() {
    const data = this.dynamic ? `*data` : `data[${this.sz()}]`;
    return emit(["struct", "{", indent(`${this.type.c()} ${data};`, 1), "}"]);
  }
  sz = () => `${this.hi.v([])} - ${this.lo.v([])} + 1`;
  zero(code) {
    if (!this.dynamic)
      return "{0}";
    const r = this.hi.context().R();
    code.push(`void *${r} AUTOFREE_ARRAY = malloc(sizeof(${this.type.c()}) * (${this.sz()}));`);
    return `{ .data = ${r} }`;
  }
  typedef = (alias) => `typedef ${this.c()} ${alias}`;
}

class StructField {
  token;
  scope;
  name;
  type;
  constructor(token, scope, name, type) {
    this.token = token;
    this.scope = scope;
    this.name = name;
    this.type = type;
  }
  c = () => `${this.type.c()} ${this.name}`;
}

class StructType extends Type {
  fields;
  constructor(fields) {
    super();
    this.fields = fields;
  }
  c() {
    const v = ["struct", "{", ...this.fields.map((f) => indent(f.c() + ";", 1)), "}"];
    return emit(v);
  }
  init = () => "{0}";
  zero = () => "{0}";
  typedef = (alias) => `typedef ${this.c()} ${alias}`;
}

class AliasType extends Type {
  reference_name;
  reference_type;
  constructor(name, ref) {
    super();
    this.reference_name = name;
    this.reference_type = ref;
  }
  c = () => this.reference_name;
  zero = () => this.reference_type.zero([]);
  typedef = (alias) => `typedef ${this.c()} ${alias}`;
}

class Node {
  token;
  scope;
  constructor(token, scope) {
    this.token = token;
    this.scope = scope;
  }
  context = () => this.token.context;
  c() {
    throw new GenerateError(`c() not implemented for ${this.constructor.name} at ${this.token}`);
  }
  toString = () => printer(this);
}

class Statement extends Node {
}

class Expression extends Node {
  type;
  constructor(token, scope, type) {
    super(token, scope);
    this.type = type;
  }
  v(code) {
    throw new GenerateError(`v() not implemented for ${this.constructor.name} at ${this.token}`);
  }
}
class TYPEIS extends Node {
  name;
  definition;
  constructor(token, scope, name, definition) {
    super(token, scope);
    this.name = name;
    this.definition = definition;
  }
}

class DECLARE extends Node {
  names;
  type;
  constructor(token, scope, names, type) {
    super(token, scope);
    this.names = names;
    this.type = type;
  }
  v(code) {
    function zero(n, type) {
      return `${type.c()} ${n} = ${type.zero(code)};`;
    }
    return this.names.map((n) => zero(n, this.type)).join(`
`);
  }
}

class Segment extends Node {
  types;
  variables;
  subroutines;
  statements;
  constructor(token, scope, types, variables, subs, statements) {
    super(token, scope);
    this.types = types;
    this.variables = variables;
    this.subroutines = subs;
    this.statements = statements;
  }
  c(main = false) {
    const v = [];
    if (this.variables) {
      for (const variable of this.variables) {
        const c = variable.v(v);
        if (main)
          this.context().common.push(c);
        else
          v.push(c);
      }
    }
    if (this.statements) {
      for (const statement of this.statements)
        v.push(statement.c());
    }
    return emit(v);
  }
}

class Argument {
  token;
  name;
  type;
  constructor(token, name, type) {
    this.token = token;
    this.name = name;
    this.type = type;
  }
  c() {
    return `${this.type.c()} ${this.name}`;
  }
}

class PROCEDURE extends Node {
  name;
  arguments;
  segment;
  constructor(token, scope, name, args, seg) {
    super(token, scope);
    this.name = name;
    this.arguments = args;
    this.segment = seg;
  }
  c() {
    const procedure = this.context().procedures[this.name];
    for (let i = 0;i < this.arguments.length; i++) {
      if (this.arguments[i].type.constructor.name !== procedure.arguments[i].type.constructor.name) {
        throw new GenerateError(`type mismatch in PROCEDURE call: ` + `${this.arguments[i].type.constructor.name} !== ${procedure.arguments[i].type.constructor.name} ` + `at ${this.token}`);
      }
    }
    const args = this.arguments.map((v2) => v2.c()).join(", ");
    const v = [`void ${this.name}(${args})`, "{"];
    this.segment.context().enter_frame();
    v.push(indent(this.segment.c(), 1));
    this.segment.context().leave_frame(v);
    v.push("}");
    return emit(v);
  }
}

class FUNCTION extends Node {
  name;
  type;
  arguments;
  segment;
  constructor(token, scope, name, type, args, seg) {
    super(token, scope);
    this.name = name;
    this.type = type;
    this.arguments = args;
    this.segment = seg;
  }
  c() {
    const func = this.context().functions[this.name];
    if (func instanceof FUNCTION) {
      for (let i = 0;i < this.arguments.length; i++) {
        if (this.arguments[i].type.constructor.name !== func.arguments[i].type.constructor.name) {
          throw new GenerateError(`type mismatch in FUNCTION call: ` + `${this.arguments[i].type.constructor.name} !== ${func.arguments[i].type.constructor.name} ` + `at ${this.token}`);
        }
      }
    }
    const type = (func instanceof BuiltinFunction ? func.type : func.type).c();
    const args = this.arguments.map((a) => a.c()).join(", ");
    const v = [`${type} ${this.name}(${args})`, "{"];
    this.segment.context().enter_frame();
    v.push(indent(this.segment.c(), 1));
    this.segment.context().leave_frame(v);
    v.push("}");
    return emit(v);
  }
}

class LABEL extends Node {
  name;
  constructor(token, scope, name) {
    super(token, scope);
    this.name = name;
  }
  c = () => `${this.name}:`;
}

class Variable {
  token;
  name;
  type;
  zero;
  constructor(token, name, type, zero) {
    this.token = token;
    this.name = name;
    this.type = type;
    this.zero = zero;
  }
  c() {
    return `${this.type.c()} ${this.name}`;
  }
  isConst() {
    return this.zero !== undefined;
  }
  const() {
    if (!this.isConst())
      throw new GenerateError(`variable '${this.name}' is not a constant at ${this.token}`);
    const z = (this.zero ?? "").replace(/"/g, "\\\"");
    return `${this.type.c()} ${this.name} = { .data = "${z}", .sz = ${z.length}, .immutable = 1 }`;
  }
  s(scope) {
    return [this.name, scope, this.type.constructor.name, String(this.token)];
  }
}

class BuiltinFunction {
  name;
  type;
  constructor(name, type) {
    this.name = name;
    this.type = type;
  }
}

class VariableField {
  token;
  name;
  constructor(token, name) {
    this.token = token;
    this.name = name;
  }
  c() {
    return `.${this.name}`;
  }
}

class VariableSubscript {
  token;
  value;
  constructor(token, value) {
    this.token = token;
    this.value = value;
  }
  index() {
    return this.value.c();
  }
}

class VariableReference extends Expression {
  scope;
  name;
  parts;
  constructor(token, scope, name, parts, type) {
    super(token, scope, type);
    this.scope = scope;
    this.name = name;
    this.parts = parts;
  }
  v(code) {
    const variable = find_existing_variable(this);
    const { reference } = expand_variable_reference(variable, this, code);
    return reference;
  }
  context = () => this.token.context;
}

class BuiltinLiteral extends Expression {
  format() {
    throw new GenerateError(`format() not implemented for ${this.constructor.name} at ${this.token}`);
  }
}

class IntegerLiteral extends BuiltinLiteral {
  value;
  constructor(token, scope, value) {
    super(token, scope, new IntegerType);
    this.value = value;
  }
  c = () => String(this.value);
  v = (code) => this.c();
  format = () => "i";
}

class RealLiteral extends BuiltinLiteral {
  value;
  constructor(token, scope, value) {
    super(token, scope, new RealType);
    this.value = value;
  }
  c() {
    const v = this.value;
    return v.includes(".") || v.includes("e") ? v : `${v}.0`;
  }
  v = (code) => this.c();
  format = () => "r";
}

class BoolLiteral extends BuiltinLiteral {
  value;
  constructor(token, scope, value) {
    super(token, scope, new BooleanType);
    this.value = value;
  }
  c = () => this.value ? "TRUE" : "FALSE";
  v = (code) => this.c();
  format = () => "b";
}

class SET extends Statement {
  target;
  expression;
  constructor(token, scope, target, expr) {
    super(token, scope);
    this.target = target;
    this.expression = expr;
  }
  c() {
    const code = [];
    for (const target of this.target) {
      const variable = find_existing_variable(target);
      const { reference, type } = expand_variable_reference(variable, target, code);
      if (type.constructor.name !== this.expression.type.constructor.name) {
        throw new GenerateError(`type mismatch in SET: ${type.constructor.name} !== ${this.expression.type.constructor.name} at ${this.token}`);
      }
      const value = this.expression.v(code);
      code.push(`${reference} = ${value};`);
    }
    return emit(code);
  }
}

class IF extends Statement {
  condition;
  then_branch;
  else_branch;
  constructor(token, scope, cond, then_, else_) {
    super(token, scope);
    this.condition = cond;
    this.then_branch = then_;
    this.else_branch = else_ ?? null;
  }
  c() {
    const code = [];
    let condition = this.condition.v(code);
    if (condition.startsWith("(") && condition.endsWith(")"))
      condition = condition.slice(1, -1);
    code.push(`if (${condition})`, "{");
    this.then_branch.context().enter_frame();
    code.push(indent(this.then_branch.c(), 1));
    this.then_branch.context().leave_frame(code);
    code.push("}");
    if (this.else_branch) {
      code.push("else", "{");
      this.else_branch.context().enter_frame();
      code.push(indent(this.else_branch.c(), 1));
      this.else_branch.context().leave_frame(code);
      code.push("}");
    }
    return emit(code);
  }
}

class FOR extends Statement {
  variable;
  init;
  segment;
  by;
  to;
  condition;
  constructor(token, scope, variable, init, doX, by, to, cond) {
    super(token, scope);
    this.variable = variable;
    this.init = init;
    this.segment = doX;
    this.by = by ?? null;
    this.to = to ?? null;
    this.condition = cond ?? null;
  }
  c() {
    const code = [];
    const init = this.init.v(code);
    code.push(`${this.variable.v([])} = ${init};`);
    const inner = [];
    const conditions = [];
    code.push("while (1)", "{");
    this.segment.context().enter_frame();
    if (this.condition)
      conditions.push(`${this.condition.v(inner)}`);
    if (this.to)
      conditions.push(`${this.variable.v([])} <= ${this.to.v(inner)}`);
    const condition = conditions.join(" && ");
    const by = this.by ? this.by.v(inner) : "1";
    const increment = `${this.variable.v([])} += ${by};`;
    code.push(indent(emit(inner), 1), indent(`if (!(${condition})) break;`, 1), indent(this.segment.c(), 1), indent(increment, 1));
    this.segment.context().leave_frame(code);
    code.push("}");
    return emit(code);
  }
}

class SELECT extends Statement {
  expr;
  cases;
  constructor(token, scope, expression, cases) {
    super(token, scope);
    this.expr = expression;
    this.cases = cases;
  }
  c() {
    const code = [];
    const preable = [];
    for (let i = 0;i < this.cases.length; i++) {
      const [condition, segment] = this.cases[i];
      if (condition) {
        const value = condition.v(preable);
        code.push((i > 0 ? "else " : "") + `if (${value})`);
      } else {
        code.push("else");
      }
      code.push("{");
      segment.context().enter_frame();
      code.push(indent(segment.c(), 1));
      segment.context().leave_frame(code);
      code.push("}");
    }
    return emit([...preable, ...code]);
  }
}

class INPUT extends Statement {
  variable_references;
  constructor(token, scope, variable_references) {
    super(token, scope);
    this.variable_references = variable_references;
  }
  c() {
    const code = [];
    for (const variable_reference of this.variable_references) {
      const variable = find_existing_variable(variable_reference);
      const { type, reference } = expand_variable_reference(variable, variable_reference, code);
      if (type instanceof StringType) {
        code.push(`{`);
        code.push(indent(`char buf[4096];`, 1));
        code.push(indent(`scanf("%4095s", buf);`, 1));
        code.push(indent(`${reference} = make_string(buf, strlen(buf));`, 1));
        code.push(`}`);
      } else if (type instanceof IntegerType)
        code.push(`scanf("%d", &${reference});`);
      else if (type instanceof RealType)
        code.push(`scanf("%lf", &${reference});`);
      else
        throw new GenerateError(`unsupported variable '${variable}' type in INPUT at ${variable.token}`);
    }
    return emit(code);
  }
}

class OUTPUT extends Statement {
  arguments;
  constructor(token, scope, args) {
    super(token, scope);
    this.arguments = args;
  }
  c() {
    const code = [];
    const fmt = [];
    const parameters = this.arguments.map((a) => expression_stringer(a, fmt, "OUTPUT", code)).join(", ");
    let v = `$output("${fmt.join("")}"`;
    if (parameters.length > 0)
      v += `, ${parameters}`;
    v += ");";
    code.push(v);
    return emit(code);
  }
}

class REPEAT extends Statement {
  label;
  constructor(token, scope, label) {
    super(token, scope);
    this.label = label;
  }
  c() {
    return `goto ${this.label};`;
  }
}

class REPENT extends Statement {
  label;
  constructor(token, scope, label) {
    super(token, scope);
    this.label = label;
  }
  c() {
    return `goto ${this.label};`;
  }
}

class BEGIN extends Statement {
  segment;
  label;
  constructor(token, scope, segment, label) {
    super(token, scope);
    this.segment = segment;
    this.label = label ?? null;
  }
  c() {
    const v = ["{"];
    this.segment.context().enter_frame();
    v.push(indent(this.segment.c(), 1));
    this.segment.context().leave_frame(v);
    v.push("}");
    if (this.label)
      v.push(this.label + ":");
    return emit(v);
  }
}

class CALL extends Statement {
  name;
  arguments;
  constructor(token, scope, name, args) {
    super(token, scope);
    this.name = name;
    this.arguments = args;
  }
  c() {
    const code = [];
    const args = this.arguments.map((a) => a.v(code)).join(", ");
    code.push(`${this.name}(${args});`);
    return emit(code);
  }
}

class RETURN extends Statement {
  value;
  constructor(token, scope, value) {
    super(token, scope);
    this.value = value ?? null;
  }
  c() {
    if (!this.value)
      return "return;";
    const code = [];
    const value = this.value.v(code);
    code.push(`return ${value};`);
    return emit(code);
  }
}

class EXIT extends Statement {
  c = () => "$exit();";
}

class EMPTY extends Statement {
  c = () => "while (0);";
}
var OPERATIONS = { "|": "||", "&": "&&", "=": "==", "<>": "!=", MOD: "%", XOR: "^" };

class FunctionCall extends Expression {
  name;
  arguments;
  constructor(token, scope, type, name, args) {
    super(token, scope, type);
    this.name = name;
    this.arguments = args;
  }
  v(code) {
    const r = this.context().R();
    const args = this.arguments.map((a) => a.v(code)).join(", ");
    const type = this.type.c();
    code.push(`const ${type} ${r} = ${this.name}(${args});`);
    return r;
  }
}

class BinaryOperation extends Expression {
  operation;
  left;
  right;
  leftType;
  rightType;
  constructor(token, scope, type, leftType, rightType, operation, left, right) {
    super(token, scope, type);
    this.operation = operation;
    this.left = left;
    this.right = right;
    this.leftType = leftType;
    this.rightType = rightType;
  }
  v(code) {
    const operation = OPERATIONS[this.operation] ?? this.operation;
    const is_string = string_compare(this.left, this.right, operation, code);
    if (is_string)
      return is_string;
    const r = this.context().R();
    const left = this.left.v(code);
    const right = this.right.v(code);
    if (this.left.type.constructor !== this.right.type.constructor) {
      throw new GenerateError(`type mismatch in binary operation: ` + `${this.left.type.constructor.name} ` + `${operation} ` + `${this.right.type.constructor.name} at ${this.token}`);
    }
    const type = this.left.type;
    code.push(`const ${type.c()} ${r} = (${left} ${operation} ${right});`);
    return r;
  }
}
class UnaryOperation extends Expression {
  operation;
  expr;
  constructor(token, scope, operation, expr) {
    super(token, scope, expr.type);
    this.operation = operation;
    this.expr = expr;
  }
  v(code) {
    const r = this.context().R();
    const operation = this.operation === "NOT" ? "!" : this.operation;
    const value = this.expr.v(code);
    code.push(`const int ${r} = (${operation}${value});`);
    return r;
  }
}
function string_compare(left, right, operation, code) {
  if (operation !== "==" && operation !== "!=")
    return null;
  function is_string_type(e) {
    if (e instanceof VariableReference) {
      const variable = find_existing_variable(e);
      const { type, reference } = expand_variable_reference(variable, e, []);
      return [type instanceof StringType, reference];
    }
    if (e.type instanceof StringType)
      return [true, e.v(code)];
    if (e instanceof FunctionCall) {
      const func = e.context().functions[e.name];
      if (!func)
        throw new GenerateError(`unknown function '${e.name}' at ${e.token}`);
      if (func.type instanceof StringType)
        return [true, e.v(code)];
    }
    return [false, null];
  }
  const [left_string, left_reference] = is_string_type(left);
  const [right_string, right_reference] = is_string_type(right);
  if (left_string || right_string) {
    const cmp = operation === "!=" ? "!=" : "==";
    const r = left.context().R();
    const left_sz = `${left_reference}.sz`;
    const right_sz = `${right_reference}.sz`;
    console.log(right_reference);
    code.push(`const int ${r} = ` + `${left_sz} == ${right_sz} ` + `&& ` + `memcmp(${left_reference}.data, ${right_reference}.data, ${left_sz}) ${cmp} 0;`);
    return r;
  }
  return null;
}

class Lexer {
  context;
  input;
  text;
  position = 0;
  line = 1;
  character = 1;
  n;
  constructor(context) {
    this.context = context;
    this.input = context.text;
    this.text = this.input.text;
    this.n = this.text.length;
  }
  peek(k = 1) {
    const j = this.position + k;
    return j < this.n ? this.text[j] : "";
  }
  current = () => this.position < this.n ? this.text[this.position] : "";
  advance(k = 1) {
    for (let i = 0;i < k; i++) {
      if (this.position < this.n) {
        const c = this.text[this.position++];
        if (c === `
`) {
          this.line += 1;
          this.character = 1;
        } else
          this.character += 1;
      }
    }
  }
  skip_whitespace_and_comments() {
    while (true) {
      while (this.current() && /\s/.test(this.current()))
        this.advance();
      const comments = () => {
        if (this.current() === "/" && this.peek() === "*") {
          this.advance(2);
          while (!(this.current() === "*" && this.peek() === "/")) {
            if (this.position >= this.n) {
              const loc = `${this.input.filename}:${this.line}:${this.character}`;
              throw new LexerError(`unterminated /* */ comment at ${loc}`);
            }
            if (comments())
              continue;
            this.advance();
          }
          this.advance(2);
          return true;
        }
        return false;
      };
      if (comments())
        continue;
      if (this.current() === "/" && this.peek() === "/") {
        this.advance(2);
        while (this.current() && this.current() !== `
`)
          this.advance();
        if (this.current() === `
`)
          this.advance();
        continue;
      }
      break;
    }
  }
  number() {
    const { line, character } = this;
    let value = "";
    while (/\d/.test(this.current())) {
      value += this.current();
      this.advance();
    }
    if (this.current() === "." || this.current().toLowerCase() === "e") {
      value += this.current();
      this.advance();
      while (/\d/.test(this.current()) || /[+\-eE]/.test(this.current())) {
        value += this.current();
        this.advance();
      }
      return new Token("REAL", value, line, character, this.context);
    }
    return new Token("INTEGER", value, line, character, this.context);
  }
  ident_or_keyword() {
    const { line, character } = this;
    const c = this.current();
    let value = "";
    if (/[A-Za-z_]/.test(c)) {
      value += c;
      this.advance();
      while (/[A-Za-z0-9_]/.test(this.current())) {
        value += this.current();
        this.advance();
      }
    }
    if (KEYWORDS.has(value))
      return new Token("KEYWORD", value, line, character, this.context);
    return new Token("IDENT", value, line, character, this.context);
  }
  string() {
    const { line, character } = this;
    const quote = this.current();
    this.advance();
    let value = "";
    while (true) {
      const c = this.current();
      if (!c) {
        const location = `${this.input.filename}:${line}:${character}`;
        throw new LexerError(`unterminated string at ${location}`);
      }
      if (c === quote) {
        this.advance();
        if (this.current() === quote) {
          value += quote;
          this.advance();
          continue;
        }
        break;
      }
      value += c;
      this.advance();
    }
    return new Token("STRING", value, line, character, this.context);
  }
  symbol() {
    const { line, character } = this;
    const two = this.current() + this.peek();
    if (SYMBOLS.has(two)) {
      this.advance(2);
      return new Token("SYMBOL", two, line, character, this.context);
    }
    const one = this.current();
    if (SYMBOLS.has(one)) {
      this.advance();
      return new Token("SYMBOL", one, line, character, this.context);
    }
    const location = `${this.input.filename}:${line}:${character}`;
    throw new LexerError(`unknown symbol '${one}' at ${location}`);
  }
  tokenize() {
    const tokens = [];
    while (true) {
      this.skip_whitespace_and_comments();
      if (this.position >= this.n)
        break;
      const c = this.current();
      if (/\d/.test(c))
        tokens.push(this.number());
      else if (/[A-Za-z_]/.test(c))
        tokens.push(this.ident_or_keyword());
      else if (c === '"')
        tokens.push(this.string());
      else
        tokens.push(this.symbol());
    }
    tokens.push(new Token("EOF", "", this.line, this.character, this.context));
    return tokens;
  }
}

class GenerateError extends Error {
  constructor(message) {
    super(`compiler error: ${message}`);
  }
}
function expression_stringer(e, fmt, callee, code) {
  const c = e.v(code);
  if (e instanceof BuiltinLiteral) {
    fmt.push(e.format());
    return c;
  }
  if (e instanceof VariableReference) {
    const variable = find_existing_variable(e);
    let { type, reference } = expand_variable_reference(variable, e, code);
    if (type instanceof AliasType)
      type = type.reference_type;
    if (!(type instanceof BuiltinType))
      throw new GenerateError(`unsupported ${callee} variable argument '${e}' of type '${type.constructor.name}' at ${e.token}`);
    fmt.push(type.format());
    return reference;
  }
  if (e instanceof ConcatenationOperation) {
    fmt.push("A");
    return c;
  }
  if (e instanceof FunctionCall) {
    const func = e.context().functions[e.name];
    const type = func instanceof BuiltinFunction ? func.type : func.type;
    if (!(type instanceof BuiltinType))
      throw new GenerateError(`unsupported ${callee} function argument ${e} of type ${type.constructor.name} at ${e.token}`);
    fmt.push(type.format());
    return c;
  }
  throw new GenerateError(`unsupported ${callee} argument '${e}' at ${e.token}`);
}
function expand_variable_reference(variable, variable_reference, code) {
  let type = variable.type;
  let reference = variable.name;
  const owner = { name: variable.name, type: variable.type };
  for (const part of variable_reference.parts) {
    if (part instanceof VariableSubscript) {
      if (type instanceof AliasType)
        type = type.reference_type;
      if (!(type instanceof ArrayType))
        throw new GenerateError(`expect ArrayType in reference type of subscript, not ${type.constructor.name} at ${part.token}`);
      const { filename } = part.token.context.text;
      const { line, character } = part.token;
      enlist_variable(new Variable(part.token, "$F", new StringType, filename), "");
      const lo = type.lo.v(code);
      const hi = type.hi.v(code);
      const index = part.value.v(code);
      code.push(`$index(${index}, ${lo}, ${hi}, &$F, ${line}, ${character});`);
      type = type.type;
      reference += `.data[(${index}) - (${lo})]`;
    } else if (part instanceof VariableField) {
      if (type instanceof AliasType)
        type = type.reference_type;
      if (!(type instanceof StructType))
        throw new GenerateError(`expect StructType in reference type of field, not ${type.constructor.name} at ${part.token}`);
      const field = type.fields.find((f) => f.name === part.name);
      if (!field)
        throw new GenerateError(`field '${part.name}' not found in ${type} at ${part.token}`);
      type = field.type;
      reference += part.c();
    } else {
      throw new GenerateError(`unexpected variable reference part '${part}' at ${variable.token}`);
    }
  }
  return { type, reference, owner };
}
function find_existing_variable(v) {
  const { context } = v.token;
  const parts = v.scope.split(".");
  let variable;
  while (parts.length) {
    const scoped_name = parts.join(".") + "|" + v.name;
    variable = context.variables[scoped_name];
    if (variable)
      break;
    parts.pop();
  }
  if (!variable)
    throw new GenerateError(`undefined variable '${v.name}' in scope '${v.scope}' at ${v.token}`);
  return variable;
}
function enlist_type(name, type, context) {
  if (context.types[name])
    throw new GenerateError(`type '${name}' already defined at ${name}`);
  context.types[name] = type;
}
function enlist_variable(variable, scope) {
  const scopedName = scope + "|" + variable.name;
  variable.token.context.variables[scopedName] = variable;
}

class Parser {
  context;
  tokens;
  i = 0;
  scopes = [];
  constructor(context) {
    this.context = context;
    this.tokens = context.tokens;
  }
  scope = () => this.scopes.length ? this.scopes.join(".") : "@";
  enter_scope = (name) => this.scopes.push(name);
  leave_scope = () => this.scopes.pop();
  current = () => this.tokens[this.i];
  peek() {
    const current = this.current();
    return this.i + 1 < this.tokens.length ? this.tokens[this.i + 1] : new Token("EOF", "", current.line, current.character, this.context);
  }
  eat(kind) {
    const kinds = Array.isArray(kind) ? kind : [kind];
    const token = this.current();
    if (kinds.includes(token.type) || kinds.includes(token.value)) {
      this.i += 1;
      return token;
    }
    const expected = kinds.join("/");
    throw new ParseError(`expected '${expected}', found '${token.value}'`, token);
  }
  accept(kind) {
    const kinds = Array.isArray(kind) ? kind : [kind];
    const token = this.current();
    if (kinds.includes(token.type) || kinds.includes(token.value)) {
      this.i += 1;
      return token;
    }
    return null;
  }
  program() {
    const token = this.current();
    this.eat("PROGRAM");
    const name = this.eat("IDENT").value;
    this.eat(":");
    this.enter_scope(`PROGRAM:${name}`);
    const segments = this.segment();
    this.leave_scope();
    this.eat("END");
    this.eat("PROGRAM");
    this.eat(name);
    this.eat(";");
    this.eat("EOF");
    return new PROGRAM(token, this.scope(), name, segments);
  }
  segment() {
    const token = this.current();
    const types = this.types_section();
    const variables = this.variables_section();
    const subroutines = this.procedures_and_functions_section();
    const statements = this.statements_section();
    return new Segment(token, this.scope(), types, variables, subroutines, statements);
  }
  types_section() {
    const out = [];
    let token;
    while (token = this.accept("TYPE")) {
      const name = this.eat("IDENT").value;
      this.eat("IS");
      const definition = this.parse_type();
      this.eat(";");
      out.push(new TYPEIS(token, this.scope(), name, definition));
      enlist_type(name, definition, this.context);
    }
    return out;
  }
  variables_section() {
    const declarations = [];
    while (true) {
      const declareToken = this.accept("DECLARE");
      if (!declareToken)
        break;
      const token = this.current();
      if (token.type === "IDENT") {
        const name = this.eat("IDENT").value;
        const type = this.parse_type();
        this.eat(";");
        declarations.push(new DECLARE(declareToken, this.scope(), [name], type));
        enlist_variable(new Variable(token, name, type), this.scope());
        continue;
      }
      if (token.value === "(") {
        this.eat("(");
        const names = [];
        while (this.current().value !== ")") {
          this.accept(",");
          names.push(this.eat("IDENT").value);
        }
        this.eat(")");
        const type = this.parse_type();
        this.eat(";");
        declarations.push(new DECLARE(declareToken, this.scope(), names, type));
        for (const name of names)
          enlist_variable(new Variable(token, name, type), this.scope());
        continue;
      }
      throw new ParseError("expected a variable or '(' variable')'", token);
    }
    return declarations;
  }
  parse_type() {
    const token = this.current();
    if (["INTEGER", "BOOLEAN", "REAL", "STRING"].includes(token.value)) {
      this.eat(token.value);
      return this.context.types[token.value];
    }
    if (token.value === "ARRAY") {
      this.eat("ARRAY");
      this.eat("[");
      let end = this.expression();
      let start = new IntegerLiteral(token, this.scope(), 0);
      if (this.accept(":")) {
        start = end;
        end = this.expression();
      }
      this.eat("]");
      this.eat("OF");
      const elementType = this.parse_type();
      const dynamic = !(end instanceof IntegerLiteral && start instanceof IntegerLiteral);
      return new ArrayType(elementType, end, start, dynamic);
    }
    if (token.value === "STRUCTURE") {
      this.eat("STRUCTURE");
      const fields = [];
      const fieldToken = this.eat("FIELD");
      let name = this.eat("IDENT").value;
      this.eat("IS");
      let fieldType = this.parse_type();
      fields.push(new StructField(fieldToken, this.scope(), name, fieldType));
      while (this.accept(",")) {
        const ft = this.eat("FIELD");
        name = this.eat("IDENT").value;
        this.eat("IS");
        fieldType = this.parse_type();
        fields.push(new StructField(ft, this.scope(), name, fieldType));
      }
      this.eat("END");
      this.eat("STRUCTURE");
      return new StructType(fields);
    }
    const typeAliasToken = this.eat("IDENT");
    const aliasName = typeAliasToken.value;
    const aliasType = this.context.types[aliasName];
    if (!aliasType)
      throw new ParseError(`unknown type alias '${aliasName}'`, typeAliasToken);
    return new AliasType(aliasName, aliasType);
  }
  procedures_and_functions_section() {
    const subroutines = [];
    while (true) {
      const is_external = this.accept("EXTERNAL");
      const token = this.accept(["FUNCTION", "PROCEDURE"]);
      if (!token)
        break;
      const name = this.eat("IDENT").value;
      this.enter_scope(token.value + ":" + name);
      let parameters = [];
      if (this.accept("(")) {
        if (this.current().value !== ")") {
          parameters = this.parameters();
          for (const p of parameters)
            enlist_variable(p, this.scope());
        }
        this.eat(")");
      }
      let type = null;
      if (token.value === "FUNCTION")
        type = this.parse_type();
      this.eat(":");
      const segment = this.segment();
      this.eat("END");
      if (is_external)
        this.eat("EXTERNAL");
      this.eat(token.value);
      this.eat(name);
      this.eat(";");
      if (token.value === "PROCEDURE") {
        const args = parameters.map((v) => new Argument(v.token, v.name, v.type));
        const procedure = new PROCEDURE(token, this.scope(), name, args, segment);
        this.context.procedures[name] = procedure;
        subroutines.push(procedure);
      } else {
        const args = parameters.map((v) => new Argument(v.token, v.name, v.type));
        const func = new FUNCTION(token, this.scope(), name, type, args, segment);
        this.context.functions[name] = func;
        subroutines.push(func);
      }
      this.leave_scope();
    }
    return subroutines;
  }
  parameters() {
    const parameters = [];
    while (true) {
      const token = this.eat("IDENT");
      const name = token.value;
      const type = this.parse_type();
      this.accept("NAME");
      const variable = new Variable(token, name, type);
      parameters.push(variable);
      enlist_variable(variable, this.scope());
      if (!this.accept(","))
        break;
    }
    return parameters;
  }
  statements_section() {
    const statements = [];
    const STATEMENTS = new Set([
      "SET",
      "CALL",
      "IF",
      "RETURN",
      "EXIT",
      "INPUT",
      "OUTPUT",
      "FOR",
      "SELECT",
      "REPEAT",
      "REPENT",
      "BEGIN",
      ";"
    ]);
    const is_label = () => {
      const current = this.current();
      return current.type === "IDENT" && current.value !== "OTHERWISE" && this.peek().value === ":";
    };
    while (STATEMENTS.has(this.current().value) || is_label()) {
      if (is_label()) {
        const token = this.eat("IDENT");
        const label = token.value;
        this.eat(":");
        statements.push(new LABEL(token, this.scope(), label));
      } else {
        statements.push(this.statement());
      }
    }
    return statements;
  }
  statement() {
    const token = this.current();
    switch (token.value) {
      case "SET":
        return this.set_statement();
      case "CALL":
        return this.call_statement();
      case "IF":
        return this.if_statement();
      case "FOR":
        return this.for_statement();
      case "SELECT":
        return this.select_statement();
      case "RETURN":
        return this.return_statement();
      case "EXIT":
        return this.exit_statement();
      case "INPUT":
        return this.input_statement();
      case "OUTPUT":
        return this.output_statement();
      case "REPEAT":
        return this.repeat_statement();
      case "REPENT":
        return this.repent_statement();
      case "BEGIN":
        return this.begin_statement();
      case ";":
        this.eat(";");
        return new EMPTY(token, this.scope());
      default:
        throw new ParseError("unexpected statement", token);
    }
  }
  arguments() {
    const args = [this.expression()];
    while (this.accept(","))
      args.push(this.expression());
    return args;
  }
  if_statement() {
    const token = this.eat("IF");
    const condition = this.expression();
    this.eat("THEN");
    this.enter_scope(token.value + ":" + this.context.S());
    const then_ = this.segment();
    this.leave_scope();
    let else_ = null;
    if (this.accept("ELSE")) {
      this.enter_scope(token.value + ":" + this.context.S());
      const else_segment = this.segment();
      this.leave_scope();
      else_ = else_segment;
    }
    this.eat("FI");
    this.eat(";");
    return new IF(token, this.scope(), condition, then_, else_);
  }
  for_statement() {
    const token = this.eat("FOR");
    const variable = this.variable_reference();
    this.eat(":=");
    const init = this.expression();
    const by = this.accept("BY") ? this.expression() : null;
    const to = this.accept("TO") ? this.expression() : null;
    const cond = this.accept("WHILE") ? this.expression() : null;
    this.eat("DO");
    this.enter_scope(token.value + ":" + this.context.S());
    const segment = this.segment();
    this.leave_scope();
    this.eat("END");
    this.eat("FOR");
    this.eat(";");
    return new FOR(token, this.scope(), variable, init, segment, by, to, cond);
  }
  select_statement() {
    const token = this.eat("SELECT");
    const expression = this.expression();
    this.eat("OF");
    const cases = [];
    while (this.accept("CASE")) {
      this.eat("(");
      const cond = this.expression();
      this.eat(")");
      this.eat(":");
      this.enter_scope(token.value + ":" + this.context.S());
      const segment = this.segment();
      this.leave_scope();
      cases.push([cond, segment]);
    }
    if (this.accept("OTHERWISE")) {
      this.eat(":");
      this.enter_scope(token.value + ":" + this.context.S());
      const segment = this.segment();
      this.leave_scope();
      cases.push([null, segment]);
    }
    this.eat("END");
    this.eat("SELECT");
    this.eat(";");
    return new SELECT(token, this.scope(), expression, cases);
  }
  return_statement() {
    const token = this.eat("RETURN");
    if (this.accept(";"))
      return new RETURN(token, this.scope(), null);
    const value = this.expression();
    this.eat(";");
    return new RETURN(token, this.scope(), value);
  }
  exit_statement() {
    const token = this.eat("EXIT");
    this.eat(";");
    return new EXIT(token, this.scope());
  }
  input_statement() {
    const token = this.eat("INPUT");
    const variable_references = [this.variable_reference()];
    while (this.accept(","))
      variable_references.push(this.variable_reference());
    this.eat(";");
    return new INPUT(token, this.scope(), variable_references);
  }
  output_statement() {
    const token = this.eat("OUTPUT");
    const expressions = [];
    if (this.current().value != ";") {
      expressions.push(this.expression());
      while (this.accept(","))
        expressions.push(this.expression());
    }
    this.eat(";");
    return new OUTPUT(token, this.scope(), expressions);
  }
  repeat_statement() {
    const token = this.eat("REPEAT");
    const label = this.eat("IDENT").value;
    this.eat(";");
    return new REPEAT(token, this.scope(), label);
  }
  repent_statement() {
    const token = this.eat("REPENT");
    const label = this.eat("IDENT").value;
    this.eat(";");
    return new REPENT(token, this.scope(), label);
  }
  begin_statement() {
    const token = this.eat("BEGIN");
    this.enter_scope(token.value + ":" + this.context.S());
    const segment = this.segment();
    this.leave_scope();
    this.eat("END");
    const label = this.accept("IDENT");
    this.eat(";");
    return new BEGIN(token, this.scope(), segment, label ? label.value : null);
  }
  set_statement() {
    const token = this.eat("SET");
    const variable = this.variable_reference();
    this.eat(":=");
    const targets = [variable];
    while (true) {
      const position = this.i;
      const tokens_ahead = [];
      while (this.current().value !== ";" && this.current().type !== "EOF") {
        tokens_ahead.push(this.current().value);
        this.i += 1;
      }
      this.i = position;
      if (!tokens_ahead.includes(":="))
        break;
      const variable2 = this.variable_reference();
      this.eat(":=");
      targets.push(variable2);
    }
    const expression = this.expression();
    this.eat(";");
    return new SET(token, this.scope(), targets, expression);
  }
  variable_reference() {
    const token = this.eat("IDENT");
    const name = token.value;
    const parts = [];
    while (true) {
      if (this.accept(".")) {
        const token2 = this.eat("IDENT");
        parts.push(new VariableField(token2, token2.value));
        continue;
      }
      if (this.accept("[")) {
        const token2 = this.current();
        const expression = this.expression();
        parts.push(new VariableSubscript(token2, expression));
        this.eat("]");
        continue;
      }
      break;
    }
    const reference = new VariableReference(token, this.scope(), name, parts, new UnknownType);
    const variable = find_existing_variable(reference);
    reference.type = expand_variable_reference(variable, reference, []).type;
    return reference;
  }
  call_statement() {
    const token = this.eat("CALL");
    const name = this.eat("IDENT").value;
    let args = [];
    if (this.accept("(")) {
      if (this.current().value !== ")")
        args = this.arguments();
      this.eat(")");
    }
    this.eat(";");
    return new CALL(token, this.scope(), name, args);
  }
  expression() {
    return this.expression_OR_XOR();
  }
  expression_OR_XOR() {
    let left = this.expression_AND();
    while (true) {
      const token = this.accept(["|", "XOR"]);
      if (!token)
        break;
      const right = this.expression_AND();
      left = new BinaryOperation(token, this.scope(), new BooleanType, left.type, right.type, token.value, left, right);
    }
    return left;
  }
  expression_AND() {
    const token = this.current();
    let left = this.expression_NOT();
    while (true) {
      const token2 = this.accept("&");
      if (!token2)
        break;
      const right = this.expression_NOT();
      left = new BinaryOperation(token2, this.scope(), new BooleanType, left.type, right.type, token2.value, left, right);
    }
    return left;
  }
  expression_NOT() {
    const token = this.accept("NOT");
    if (token)
      return new UnaryOperation(token, this.scope(), token.value, this.expression_NOT());
    return this.expression_RELATION();
  }
  expression_RELATION() {
    const token = this.current();
    let left = this.expression_CONCATENATION();
    while (true) {
      const token2 = this.accept(["<", ">", "=", "<=", ">=", "<>"]);
      if (!token2)
        break;
      const right = this.expression_CONCATENATION();
      left = new BinaryOperation(token2, this.scope(), new BooleanType, left.type, right.type, token2.value, left, right);
    }
    return left;
  }
  expression_CONCATENATION() {
    const parts = [this.expression_ADDING()];
    let token = this.current();
    while (this.accept("||")) {
      token = this.current();
      parts.push(this.expression_ADDING());
    }
    if (parts.length === 1)
      return parts[0];
    return new ConcatenationOperation(token, this.scope(), new StringType, parts);
  }
  expression_ADDING() {
    let left = this.expression_MULTIPLYING();
    while (true) {
      const token = this.accept(["+", "-"]);
      if (!token)
        break;
      const right = this.expression_MULTIPLYING();
      left = new BinaryOperation(token, this.scope(), left.type, left.type, right.type, token.value, left, right);
    }
    return left;
  }
  expression_MULTIPLYING() {
    const token = this.current();
    let left = this.expression_FUNCTION_CALL();
    while (true) {
      const token2 = this.accept(["*", "/", "MOD"]);
      if (!token2)
        break;
      const right = this.expression_FUNCTION_CALL();
      left = new BinaryOperation(token2, this.scope(), left.type, left.type, right.type, token2.value, left, right);
    }
    return left;
  }
  expression_FUNCTION_CALL() {
    const token = this.current();
    if (token.type === "IDENT" && this.peek().value === "(") {
      const name = this.eat("IDENT").value;
      this.eat("(");
      const args = this.current().value !== ")" ? this.arguments() : [];
      this.eat(")");
      const type = this.context.functions[name];
      if (!type)
        throw new ParseError(`undefined function '${name}'`, token);
      const rettype = type instanceof BuiltinFunction ? type.type : type.type;
      return new FunctionCall(token, this.scope(), rettype, name, args);
    }
    return this.factor();
  }
  factor() {
    const token = this.current();
    if (token.type === "INTEGER") {
      const token2 = this.eat("INTEGER");
      return new IntegerLiteral(token2, this.scope(), parseInt(token2.value, 10));
    }
    if (token.type === "REAL") {
      const token2 = this.eat("REAL");
      return new RealLiteral(token2, this.scope(), token2.value);
    }
    if (token.type === "STRING") {
      const token2 = this.eat("STRING");
      const context = this.context;
      const existing = Object.values(context.variables).find((v) => v.isConst() && v.zero === token2.value);
      if (existing)
        return new VariableReference(token2, "", existing.name, [], existing.type);
      const const_i = Object.values(context.variables).filter((v) => v.isConst()).length;
      const name = `$${const_i}`;
      const variable = new Variable(token2, name, new StringType, token2.value);
      enlist_variable(variable, "");
      return new VariableReference(token2, "", name, [], variable.type);
    }
    if (token.value === "+" || token.value === "-") {
      const unaryToken = this.eat(token.value);
      const factor = this.factor();
      return new UnaryOperation(unaryToken, this.scope(), unaryToken.value, factor);
    }
    if (token.value === "(") {
      this.eat("(");
      const e = this.expression();
      this.eat(")");
      return e;
    }
    if (token.value === "TRUE" || token.value === "FALSE") {
      const valueToken = this.eat(token.value);
      return new BoolLiteral(valueToken, this.scope(), valueToken.value === "TRUE");
    }
    if (token.type === "IDENT") {
      return this.variable_reference();
    }
    throw new ParseError("expected an identifier or INTEGER/REAL/BOOLEAN/STRING literal or '+', '-', '(', 'TRUE/FALSE'", token);
  }
}

class ConcatenationOperation extends Expression {
  parts;
  constructor(token, scope, type, parts) {
    super(token, scope, type);
    this.parts = parts;
  }
  v(code) {
    const r = this.context().R();
    const fmt = [];
    const args = this.parts.map((x) => expression_stringer(x, fmt, "||", code)).join(", ");
    code.push(`const STR ${r} = $concat("${fmt.join("")}", ${args});`);
    return r;
  }
}

class PROGRAM extends Node {
  name;
  segment;
  constructor(token, scope, name, segment) {
    super(token, scope);
    this.name = name;
    this.segment = segment;
  }
  c() {
    const { filename } = this.token.context.text;
    enlist_variable(new Variable(this.token, "$F", new StringType, filename), "");
    const v = ["int main_program()", "{"];
    this.segment.context().enter_frame();
    v.push(indent(this.segment.c(true), 1));
    this.segment.context().leave_frame(v);
    v.push("}");
    return emit(v);
  }
}

class Compiler {
  context;
  lexer;
  parser;
  constructor(text) {
    this.context = new Context({
      text,
      types: {
        INTEGER: new IntegerType,
        REAL: new RealType,
        BOOLEAN: new BooleanType,
        STRING: new StringType
      },
      functions: {
        LENGTH: new BuiltinFunction("LENGTH", new IntegerType),
        CHARACTER: new BuiltinFunction("CHARACTER", new StringType),
        SUBSTR: new BuiltinFunction("SUBSTR", new StringType),
        FIX: new BuiltinFunction("FIX", new IntegerType),
        FLOAT: new BuiltinFunction("FLOAT", new RealType),
        FLOOR: new BuiltinFunction("FLOOR", new IntegerType)
      },
      procedures: {},
      variables: {},
      flags: {},
      tokens: [],
      common: []
    });
  }
  compile() {
    this.lexer = new Lexer(this.context);
    this.context.tokens = this.lexer.tokenize();
    this.parser = new Parser(this.context);
    return this.parser.program();
  }
}
function compileToC(source, filename = "source.easy") {
  const compiler = new Compiler(new InputText({ text: source, filename }));
  const context = compiler.context;
  const firstLine = source.split(/\r?\n/)[0]?.trim() ?? "";
  if (firstLine.startsWith("//flags ")) {
    const pairs = firstLine.split(/\s+/).slice(1);
    const flags = Object.fromEntries(pairs.map((v) => v.split("=")));
    Object.assign(context.flags, flags);
  }
  const program = compiler.compile();
  const compiledText = program.c().trim();
  const output = [];
  output.push(`#include "runtime.c"
`);
  for (const [name, definition] of Object.entries(context.types))
    output.push(definition.typedef(name) + `;
`);
  if (context.common.length)
    output.push(emit(context.common) + `
`);
  for (const v of Object.values(context.variables))
    if (v.isConst())
      output.push(v.const() + `;
`);
  for (const f of Object.values(context.functions)) {
    if (f instanceof BuiltinFunction)
      continue;
    output.push(f.c() + `
`);
  }
  for (const v of Object.values(context.procedures))
    output.push(v.c() + `
`);
  output.push(compiledText + `
`);
  return output.join("");
}
if (false) {}

// docs/build-info.ts
var BUILD_TIME = "2026-04-24 11:23:23";

// docs/playground.ts
var fetchExample = (f) => fetch(`examples/${f}`).then((r) => r.text());
var EXAMPLES = (window.easyExamples ?? []).map((e) => {
  const ex = {
    name: e.name,
    filename: e.filename,
    source: fetchExample(e.filename)
  };
  ex.source.then((s) => {
    ex.resolvedSource = s;
    renderTabs();
  }, () => {});
  return ex;
});
function tabMatchesExample(t) {
  const ex = EXAMPLES.find((e) => e.filename === t.filename);
  return !!ex && ex.resolvedSource === t.source;
}
var TABS_KEY = "easy-playground:tabs";
var ACTIVE_KEY = "easy-playground:active";
var THEME_KEY = "easy-playground:theme";
var FORMAT_KEY = "easy-playground:format";
var DEFAULT_FILENAME = "program.easy";
var DEFAULT_EXAMPLE = "sieve";
var OUTPUT_FORMATS = ["easy", "c"];
var DEFAULT_FORMAT = "easy";
var tabs = [];
var active = 0;
function applyTheme(t) {
  document.body.classList.toggle("theme-light", t === "light");
}
function loadTheme() {
  try {
    return localStorage.getItem(THEME_KEY) === "dark" ? "dark" : "light";
  } catch {
    return "light";
  }
}
function saveTheme(t) {
  try {
    localStorage.setItem(THEME_KEY, t);
  } catch {}
}
var source = document.getElementById("source");
var cOut = document.getElementById("c-output");
var cTitle = document.getElementById("c-title");
var errorEl = document.getElementById("error");
var select = document.getElementById("example");
var confirmModal = document.getElementById("confirm-modal");
var confirmMessage = document.getElementById("confirm-message");
var confirmOk = document.getElementById("confirm-ok");
var confirmCancel = document.getElementById("confirm-cancel");
var uploadBtn = document.getElementById("upload-btn");
var downloadBtn = document.getElementById("download-btn");
var downloadFormatSel = document.getElementById("download-format");
var resetBtn = document.getElementById("reset");
var themeBtn = document.getElementById("theme");
var fileInput = document.getElementById("file-input");
var filenameInput = document.getElementById("filename");
var tabsEl = document.getElementById("tabs");
function easyName() {
  return filenameInput.value.trim() || DEFAULT_FILENAME;
}
function stem(name) {
  return name.replace(/\.[^.]*$/, "") || name;
}
function outputName(format2) {
  return `${stem(easyName())}.${format2}`;
}
for (const ex of EXAMPLES) {
  const opt = document.createElement("option");
  opt.value = ex.name;
  opt.textContent = ex.name;
  select.appendChild(opt);
}
select.addEventListener("change", async () => {
  const ex = EXAMPLES.find((e) => e.name === select.value);
  if (!ex)
    return;
  const exSource = await ex.source;
  tabs[active].source = source.value;
  const uniqueName = uniqueFilename(ex.filename);
  tabs.push({ filename: uniqueName, source: exSource });
  active = tabs.length - 1;
  source.value = exSource;
  filenameInput.value = uniqueName;
  lastGoodName = uniqueName;
  source.scrollTop = 0;
  saveTabs();
  renderTabs();
  onChange();
  source.focus();
});
function uniqueFilename(base) {
  if (!tabs.some((t, i) => i !== active && t.filename === base))
    return base;
  const m = base.match(/^(.*?)(\.[^.]*)?$/);
  const s = m ? m[1] : base;
  const ext = m && m[2] ? m[2] : "";
  let n = 2;
  while (tabs.some((t, i) => i !== active && t.filename === `${s}-${n}${ext}`))
    n++;
  return `${s}-${n}${ext}`;
}
function deselectExample() {
  if (select.value)
    select.value = "";
}
source.addEventListener("input", deselectExample);
filenameInput.addEventListener("input", deselectExample);
var confirmResolver = null;
function askConfirm(message) {
  confirmMessage.textContent = message;
  confirmModal.hidden = false;
  confirmOk.focus();
  return new Promise((resolve2) => {
    confirmResolver = resolve2;
  });
}
function closeConfirm(result) {
  confirmModal.hidden = true;
  const r = confirmResolver;
  confirmResolver = null;
  if (r)
    r(result);
}
confirmOk.addEventListener("click", () => closeConfirm(true));
confirmCancel.addEventListener("click", () => closeConfirm(false));
confirmModal.addEventListener("click", (e) => {
  if (e.target === confirmModal)
    closeConfirm(false);
});
window.addEventListener("keydown", (e) => {
  if (e.key === "Escape" && !confirmModal.hidden)
    closeConfirm(false);
  if (e.key === "Enter" && !confirmModal.hidden) {
    e.preventDefault();
    closeConfirm(true);
  }
});
var lastC = null;
function setError(msg) {
  errorEl.textContent = msg;
  errorEl.classList.add("visible");
}
function clearError() {
  errorEl.textContent = "";
  errorEl.classList.remove("visible");
}
function formatCompileError(e) {
  if (e instanceof ParseError)
    return String(e);
  if (e instanceof LexerError || e instanceof GenerateError || e instanceof CompilerError) {
    return e.message;
  }
  return e.message ?? String(e);
}
function runPipeline() {
  const src = source.value;
  const file = easyName();
  let cText;
  try {
    cText = compileToC(src, file);
  } catch (e) {
    lastC = null;
    cOut.textContent = "";
    cTitle.textContent = "output.c";
    setError(formatCompileError(e));
    updateDownloadEnabled();
    return;
  }
  clearError();
  lastC = cText;
  cOut.textContent = cText;
  const lines = cText.split(/\r?\n/).length;
  cTitle.textContent = `${stem(file)}.c — ${cText.length} B, ${lines} lines`;
  updateDownloadEnabled();
}
function saveTabs() {
  try {
    localStorage.setItem(TABS_KEY, JSON.stringify(tabs));
    localStorage.setItem(ACTIVE_KEY, String(active));
  } catch {}
}
function save() {
  tabs[active].source = source.value;
  saveTabs();
}
function renderTabs() {
  tabsEl.innerHTML = "";
  tabs.forEach((t, i) => {
    const el = document.createElement("div");
    const live = i === active ? source.value : t.source;
    const matches = tabMatchesExample({ filename: t.filename, source: live });
    el.className = "tab" + (i === active ? " active" : "") + (matches ? " example" : " modified");
    el.title = t.filename;
    const name = document.createElement("span");
    name.textContent = t.filename || "(untitled)";
    el.appendChild(name);
    const close = document.createElement("button");
    close.type = "button";
    close.className = "close";
    close.textContent = "×";
    close.title = "close tab";
    close.addEventListener("click", (e) => {
      e.stopPropagation();
      closeTab(i);
    });
    el.appendChild(close);
    el.addEventListener("click", () => switchTab(i));
    tabsEl.appendChild(el);
  });
  const add = document.createElement("button");
  add.type = "button";
  add.className = "tab-add";
  add.textContent = "+";
  add.title = "new tab";
  add.addEventListener("click", () => newTab());
  tabsEl.appendChild(add);
}
function nextUntitled() {
  let n = 1;
  while (tabs.some((t) => t.filename === `untitled-${n}.easy`))
    n++;
  return `untitled-${n}.easy`;
}
function switchTab(i) {
  if (i === active || i < 0 || i >= tabs.length)
    return;
  tabs[active].source = source.value;
  active = i;
  source.value = tabs[active].source;
  filenameInput.value = tabs[active].filename;
  lastGoodName = tabs[active].filename;
  source.scrollTop = 0;
  saveTabs();
  renderTabs();
  deselectExample();
  runPipeline();
  source.focus();
}
function newTab() {
  tabs[active].source = source.value;
  tabs.push({ filename: nextUntitled(), source: "" });
  active = tabs.length - 1;
  source.value = "";
  filenameInput.value = tabs[active].filename;
  lastGoodName = tabs[active].filename;
  source.scrollTop = 0;
  saveTabs();
  renderTabs();
  deselectExample();
  runPipeline();
  source.focus();
}
async function closeTab(i) {
  const current = i === active ? source.value : tabs[i].source;
  const matchesExample = tabMatchesExample({
    filename: tabs[i].filename,
    source: current
  });
  if (current.trim().length > 0 && !matchesExample) {
    const ok = await askConfirm(`Close "${tabs[i].filename}"? Its content will be lost.`);
    if (!ok)
      return;
  }
  if (tabs.length === 1) {
    tabs[0] = { filename: DEFAULT_FILENAME, source: "" };
    active = 0;
    source.value = "";
    filenameInput.value = tabs[0].filename;
    lastGoodName = tabs[0].filename;
  } else {
    tabs.splice(i, 1);
    if (active > i)
      active--;
    else if (active === i && active >= tabs.length)
      active = tabs.length - 1;
    source.value = tabs[active].source;
    filenameInput.value = tabs[active].filename;
    lastGoodName = tabs[active].filename;
  }
  saveTabs();
  renderTabs();
  deselectExample();
  runPipeline();
}
var lastGoodName = "";
filenameInput.addEventListener("focus", () => {
  lastGoodName = filenameInput.value;
});
filenameInput.addEventListener("input", () => {
  tabs[active].filename = filenameInput.value;
  saveTabs();
  renderTabs();
});
filenameInput.addEventListener("change", () => {
  const val = filenameInput.value.trim();
  const dup = tabs.findIndex((t, i) => i !== active && t.filename === val);
  if (!val || dup !== -1) {
    if (dup !== -1)
      alert(`A tab named "${val}" already exists.`);
    filenameInput.value = lastGoodName;
    tabs[active].filename = lastGoodName;
  } else {
    filenameInput.value = val;
    tabs[active].filename = val;
    lastGoodName = val;
  }
  saveTabs();
  renderTabs();
});
function onChange() {
  save();
  runPipeline();
  renderTabs();
}
source.addEventListener("input", onChange);
function downloadBlob(data, name, type) {
  const blob = new Blob([data], { type });
  const url = URL.createObjectURL(blob);
  const a = document.createElement("a");
  a.href = url;
  a.download = name;
  document.body.appendChild(a);
  a.click();
  a.remove();
  URL.revokeObjectURL(url);
}
function loadFormat() {
  try {
    const v = localStorage.getItem(FORMAT_KEY);
    if (v && OUTPUT_FORMATS.includes(v)) {
      return v;
    }
  } catch {}
  return DEFAULT_FORMAT;
}
function saveFormat(f) {
  try {
    localStorage.setItem(FORMAT_KEY, f);
  } catch {}
}
function selectedFormat() {
  return downloadFormatSel.value;
}
function updateDownloadEnabled() {
  const fmt = selectedFormat();
  if (fmt === "easy") {
    downloadBtn.disabled = false;
    return;
  }
  downloadBtn.disabled = lastC === null;
}
downloadFormatSel.value = loadFormat();
updateDownloadEnabled();
downloadFormatSel.addEventListener("change", () => {
  saveFormat(selectedFormat());
  updateDownloadEnabled();
});
downloadBtn.addEventListener("click", () => {
  const fmt = selectedFormat();
  if (fmt === "easy") {
    downloadBlob(source.value, easyName(), "text/plain");
    return;
  }
  if (lastC === null)
    return;
  downloadBlob(lastC, outputName("c"), "text/plain");
});
uploadBtn.addEventListener("click", () => fileInput.click());
resetBtn.addEventListener("click", async () => {
  const ok = await askConfirm("Reset the current tab to the default example? This replaces its content.");
  if (!ok)
    return;
  const def = EXAMPLES.find((e) => e.name === DEFAULT_EXAMPLE) ?? EXAMPLES[0];
  if (!def)
    return;
  const defSource = await def.source;
  const uniqueName = uniqueFilename(def.filename);
  tabs[active] = { filename: uniqueName, source: defSource };
  source.value = defSource;
  filenameInput.value = uniqueName;
  lastGoodName = uniqueName;
  select.value = def.name;
  source.scrollTop = 0;
  saveTabs();
  renderTabs();
  onChange();
  source.focus();
});
fileInput.addEventListener("change", async () => {
  const f = fileInput.files?.[0];
  if (!f)
    return;
  const text = await f.text();
  const uniqueName = uniqueFilename(f.name);
  tabs.push({ filename: uniqueName, source: text });
  active = tabs.length - 1;
  source.value = text;
  filenameInput.value = uniqueName;
  lastGoodName = uniqueName;
  source.scrollTop = 0;
  fileInput.value = "";
  saveTabs();
  renderTabs();
  onChange();
  source.focus();
});
var buildTimeEl = document.getElementById("build-time");
if (buildTimeEl && BUILD_TIME)
  buildTimeEl.textContent = BUILD_TIME;
themeBtn.addEventListener("click", () => {
  const next = document.body.classList.contains("theme-light") ? "dark" : "light";
  applyTheme(next);
  saveTheme(next);
});
applyTheme(loadTheme());
async function loadTabsFromStorage() {
  try {
    const raw = localStorage.getItem(TABS_KEY);
    if (raw) {
      const parsed = JSON.parse(raw);
      if (Array.isArray(parsed) && parsed.length > 0) {
        tabs = parsed.map((t) => ({
          filename: String(t.filename ?? DEFAULT_FILENAME),
          source: String(t.source ?? "")
        }));
        const a = Number(localStorage.getItem(ACTIVE_KEY) ?? 0) | 0;
        active = a < 0 || a >= tabs.length ? 0 : a;
        return;
      }
    }
  } catch {}
  const def = EXAMPLES.find((e) => e.name === DEFAULT_EXAMPLE) ?? EXAMPLES[0];
  const src = await def?.source ?? "";
  const name = def?.filename ?? DEFAULT_FILENAME;
  tabs = [{ filename: name, source: src }];
  active = 0;
  saveTabs();
}
(async () => {
  await loadTabsFromStorage();
  source.value = tabs[active].source;
  filenameInput.value = tabs[active].filename;
  lastGoodName = tabs[active].filename;
  renderTabs();
  onChange();
})();
