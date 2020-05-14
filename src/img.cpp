#include "img.h"
#include "vk.h"

img::img(int sx, int sy)
{
    this->im = gdImageCreateTrueColor(sx, sy);
}

img::img(gdImagePtr New)
{
    this->im = New;
}

#include "str.h"
img::img(Doc doc, Net& net)
{
    this->im = NULL;
    std::string buff = net.send(doc.url);
    if (str::low(doc.ext) == "jpg" || str::low(doc.ext) == "jpeg")
        this->im = gdImageCreateFromJpegPtr(buff.size(), (void*)buff.c_str());
    else if (str::low(doc.ext) == "png")
        this->im = gdImageCreateFromPngPtr(buff.size(), (void*)buff.c_str());
    else if (str::low(doc.ext) == "bmp")
        this->im = gdImageCreateFromBmpPtr(buff.size(), (void*)buff.c_str());
}

std::string img::getPng()
{
    int s;
    //void* png = gdImagePngPtrEx(this->im, &s, 0);
    void* png = gdImagePngPtr(this->im, &s);
    std::string buff((const char*)png, s);
    if (png)
        gdFree(png);
    return buff;
}

Doc img::getDoc(uint32_t peer_id, Net& net, Vk& vk)
{
    if (this->im == NULL)
        return Doc();
    std::string dat = this->getPng();
    Doc doc;
    if (doc.uploadDoc("img.png", dat, net, vk, peer_id))
        return doc;
    return doc;
}

Doc img::getPhoto(uint32_t peer_id, Net& net, Vk& vk)
{
    if (this->im == NULL)
        return Doc();
    std::string dat = this->getPng();
    Doc doc;
    if (doc.uploadPhoto("img.png", dat, net, vk, peer_id))
        return doc;
    return doc;
}

img::~img()
{
    if (this->im)
        gdImageDestroy(this->im);
}

img img::copy()
{
    return img(gdImageClone(this->im));
}

Doc img::CVtoPhoto(cv::Mat matIm, uint32_t peer_id, Net& net, Vk& vk)
{
    std::vector<uint8_t> buff;
    cv::imencode(".png", matIm, buff, {cv::IMWRITE_JPEG_PROGRESSIVE, 1});
    std::string imBuff(buff.begin(), buff.end());
    Doc doc;
    if (doc.uploadPhoto("img.png", imBuff, net, vk, peer_id))
        return doc;
    doc.doc_id = 0;
    return doc;
}

Doc img::CVtoDoc(cv::Mat matIm, uint32_t peer_id, Net& net, Vk& vk)
{
    std::vector<uint8_t> buff;
    cv::imencode(".png", matIm, buff, {cv::IMWRITE_JPEG_PROGRESSIVE, 1});
    std::string imBuff(buff.begin(), buff.end());
    Doc doc;
    if (doc.uploadDoc("img.png", imBuff, net, vk, peer_id))
        return doc;
    return doc;
}

cv::Mat img::getCVim()
{
    std::string data = this->getPng();
    std::vector<uint8_t> vectordata(data.begin(),data.end());
    cv::Mat data_mat(vectordata,true);
    return cv::imdecode(data_mat,1);
}

//color transformations

hsv_t rgb2hsv(rgb_t in)
{
    hsv_t out;
    double min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min < in.b ? min : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max > in.b ? max : in.b;

    out.v = max; // v
    delta = max - min;
    if (delta < 0.00001) {
        out.s = 0;
        out.h = 0; // undefined, maybe nan?
        return out;
    }
    if (max > 0.0) { // NOTE: if Max is == 0, this divide would cause a crash
        out.s = (delta / max); // s
    } else {
        // if max is 0, then r = g = b = 0
        // s = 0, h is undefined
        out.s = 0.0;
        out.h = NAN; // its now undefined
        return out;
    }
    if (in.r >= max) // > is bogus, just keeps compilor happy
        out.h = (in.g - in.b) / delta; // between yellow & magenta
    else if (in.g >= max)
        out.h = 2.0 + (in.b - in.r) / delta; // between cyan & yellow
    else
        out.h = 4.0 + (in.r - in.g) / delta; // between magenta & cyan

    out.h *= 60.0; // degrees

    if (out.h < 0.0)
        out.h += 360.0;

    return out;
}

rgb_t hsv2rgb(hsv_t in)
{
    double hh, p, q, t, ff;
    long i;
    rgb_t out;

    if (in.s <= 0.0) { // < is bogus, just shuts up warnings
        out.r = in.v;
        out.g = in.v;
        out.b = in.v;
        return out;
    }
    hh = in.h;
    if (hh >= 360.0)
        hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch (i) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }
    return out;
}

