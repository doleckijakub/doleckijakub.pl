import "./globals.css";
import Header from "@/components/layout/Header";
import Container from "@/components/layout/Container";

export const metadata = {
  title: "Jakub Dolecki",
  description: "Portfolio",
};

export default function RootLayout({
  children,
}: {
  children: React.ReactNode;
}) {
  return (
    <html lang="en">
      <body>
        <Header />
        <Container>
          {children}
        </Container>
      </body>
    </html>
  );
}