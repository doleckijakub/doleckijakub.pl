type TechIconName =
    | "rust"
    | "php"
    | "python"
    | "cplusplus"
    | "c"
    | "typescript"
    | "javascript"
    | "html5"
    | "java"
    | "docker"
    | "postgresql"
    | "linux"
    | "react"
    | "mariadb"
    | "nodejs"
    | "nextjs";

const deviconMap: Record<TechIconName, string> = {
    rust: "rust",
    php: "php",
    python: "python",
    cplusplus: "cplusplus",
    c: "c",
    typescript: "typescript",
    javascript: "javascript",
    html5: "html5",
    java: "java",
    docker: "docker",
    postgresql: "postgresql",
    linux: "linux",
    react: "react",
    mariadb: "mariadb",
    nodejs: "nodejs",
    nextjs: "nextjs",
};

export function TechIcon({
    name,
    className,
}: {
    name: TechIconName;
    className?: string;
}) {
    const icon = deviconMap[name];

    return (
        <img
            src={`https://cdn.jsdelivr.net/gh/devicons/devicon/icons/${icon}/${icon}-original.svg`}
            alt={name}
            className={className}
        />
    );
}