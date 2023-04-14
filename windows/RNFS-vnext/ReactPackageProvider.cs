using Microsoft.ReactNative.Managed;
using Microsoft.ReactNative;

namespace RNFSvnext
{
    public sealed class ReactPackageProvider : IReactPackageProvider
    {
        public void CreatePackage(IReactPackageBuilder packageBuilder)
        {
            packageBuilder.AddAttributedModules();
        }
    }
}